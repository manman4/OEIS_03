"""Check durable final summaries, independent outlier counts, and file separation."""
import fcntl
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parent
BINARY = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else ROOT / "ulam02"


def terms_by_definition(count):
    terms, sums, candidate = [1, 2], {3: 1}, 3
    while len(terms) < count:
        if sums.get(candidate, 0) == 1:
            for a in terms:
                total = a + candidate
                sums[total] = min(2, sums.get(total, 0) + 1)
            terms.append(candidate)
        candidate += 1
    return terms


def terminal_report(result):
    assert "runtime error:" not in result.stderr, result.stderr
    assert "UndefinedBehaviorSanitizer" not in result.stderr, result.stderr
    return json.loads(result.stderr.splitlines()[-1])


def invoke(directory, *args, success=True):
    result = subprocess.run([str(BINARY), *map(str, args)], cwd=directory,
                            capture_output=True, text=True, timeout=30)
    assert (result.returncode == 0) == success, result.stderr
    return result


terms = terms_by_definition(1000)
with tempfile.TemporaryDirectory(prefix="ulam02-summary-test-") as tmp:
    directory = Path(tmp)
    summary = directory / "346216_02_summary.json"
    bfile = directory / "b346216.txt"
    result = invoke(directory, "--ram", "--terms", 1000, "--report", 100)
    stats = terminal_report(result)
    assert summary.read_text() == result.stderr.splitlines(keepends=True)[-1]
    assert stats["status"] == "complete" and stats["value"] == terms[-1]
    assert stats["target_terms"] == 1000
    assert bfile.read_text() == result.stdout
    assert not list(directory.glob("*.tmp.*"))

    for p, q in [(856371966, 350477575), (22, 9), (2, 1), (3, 1)]:
        result = invoke(directory, "--ram", "--no-bfile", "--terms", 1000,
                        "--p", p, "--q", q, "--report", 0)
        stats = terminal_report(result)
        residues = [(q * value) % p for value in terms]
        low = sum(3 * r < p for r in residues)
        high = sum(3 * r > 2 * p for r in residues)
        assert stats["residue_outliers_low"] == low
        assert stats["residue_outliers_high"] == high
        assert stats["residue_outliers"] == low + high
        assert stats["p"] == p and stats["q"] == q
        assert summary.read_text() == result.stderr.splitlines(keepends=True)[-1]

    result = invoke(directory, "--ram", "--no-bfile", "--terms", 1000,
                    "--window", 1, "--anchor-limit", 0, "--dictionary-limit", 1,
                    "--report", 0)
    stats = terminal_report(result)
    assert stats["fallback_calls"] > 0 and stats["escaped_blocks"] == 0
    # Compression starts at 18,000; use 2,000 terms to force raw escapes as well.
    result = invoke(directory, "--ram", "--no-bfile", "--terms", 2000,
                    "--window", 1, "--anchor-limit", 0, "--dictionary-limit", 1,
                    "--report", 0)
    stats = terminal_report(result)
    assert stats["fallback_calls"] > 0 and stats["escaped_blocks"] > 0
    assert summary.read_text() == result.stderr.splitlines(keepends=True)[-1]

    for count, expected in [(1, 0), (2, 1)]:
        result = invoke(directory, "--ram", "--no-bfile", "--terms", count, "--report", 0)
        assert terminal_report(result)["residue_outliers"] == expected

    (directory / "nested").mkdir()
    custom = "nested/report with spaces.json"
    result = invoke(directory, "--ram", "--no-bfile", "--terms", 10,
                    "--summary", custom, "--report", 0)
    assert (directory / custom).read_text() == result.stderr.splitlines(keepends=True)[-1]
    before = summary.read_bytes()
    invoke(directory, "--ram", "--no-bfile", "--no-summary", "--terms", 10, "--report", 0)
    assert summary.read_bytes() == before

    # A normal resource stop still saves every final counter, with incomplete status.
    result = invoke(directory, "--ram", "--no-bfile", "--terms", 1000,
                    "--max-value", 100, "--window", 1, "--anchor-limit", 0,
                    "--report", 0, success=False)
    stats = terminal_report(result)
    assert stats["status"] == "incomplete" and "max-value" in stats["reason"]
    assert stats["fallback_calls"] > 0 and stats["residue_outliers"] > 0
    assert summary.read_text() == result.stderr.splitlines(keepends=True)[-1]

    # An initialization failure includes its reason with valid JSON escaping.
    unusual = directory / 'cache_"line\n.bin'
    unusual.write_text("preserve")
    result = invoke(directory, "--no-bfile", "--terms", 10, "--disk", unusual,
                    "--report", 0, success=False)
    stats = terminal_report(result)
    assert stats["setup_complete"] is False and str(unusual) in stats["reason"]
    assert unusual.read_text() == "preserve"
    assert json.loads(summary.read_text()) == stats

    # A handled signal saves an incomplete report and releases both locks.
    partial = directory / "partial.txt"
    with (directory / "signal.log").open("w") as log:
        process = subprocess.Popen([str(BINARY), "--ram", "--terms", "10000000",
                                    "--bfile", str(partial), "--window", "1",
                                    "--anchor-limit", "0", "--report", "0"],
                                   cwd=directory, stdout=subprocess.DEVNULL, stderr=log)
        try:
            deadline = time.monotonic() + 10
            while not (partial.exists() and b"1 18\n" in partial.read_bytes()):
                assert process.poll() is None and time.monotonic() < deadline
                time.sleep(0.002)
            process.send_signal(signal.SIGTERM)
            assert process.wait(timeout=10) == 1
        finally:
            if process.poll() is None:
                process.kill()
                process.wait()
    stats = json.loads(summary.read_text())
    assert stats["status"] == "incomplete" and "signal" in stats["reason"]
    assert stats["terms"] >= 10 and "fallback_calls" in stats and "escaped_blocks" in stats
    assert partial.read_bytes().endswith(b"\n")

    # Aliases must not modify the b-file, cache, or previous summary.
    protected = bfile.read_bytes()
    for path in ["b346216.txt", "./b346216.txt", "nested/../b346216.txt"]:
        invoke(directory, "--ram", "--terms", 1000, "--summary", path, success=False)
        assert bfile.read_bytes() == protected
    alias = directory / "hardlink.json"
    os.link(bfile, alias)
    invoke(directory, "--ram", "--terms", 1000, "--summary", alias, success=False)
    assert bfile.read_bytes() == protected
    for suffix in ["", ".raw"]:
        invoke(directory, "--no-bfile", "--terms", 10, "--disk", "future.bin",
               "--summary", "future.bin" + suffix, success=False)
        assert not (directory / "future.bin").exists()
    invoke(directory, "--ram", "--terms", 10, "--summary", "guard.json",
           "--bfile", "guard.json.lock", success=False)
    assert not (directory / "guard.json.lock").exists()
    link = directory / "link.json"
    link.symlink_to(summary)
    old_summary = summary.read_bytes()
    invoke(directory, "--ram", "--no-bfile", "--terms", 10, "--summary", link, success=False)
    assert summary.read_bytes() == old_summary
    with (directory / "346216_02_summary.json.lock").open("r+") as locked:
        fcntl.flock(locked, fcntl.LOCK_EX | fcntl.LOCK_NB)
        invoke(directory, "--ram", "--no-bfile", "--terms", 10, success=False)
    assert summary.read_bytes() == old_summary

    # Open in r+ so the test itself does not truncate the protected file.
    for target in [bfile, alias]:
        for fd in [1, 2]:
            with target.open("r+") as stream:
                result = subprocess.run([str(BINARY), "--ram", "--terms", "1000", "--report", "1"],
                                        cwd=directory, text=True, timeout=30,
                                        stdout=stream if fd == 1 else subprocess.PIPE,
                                        stderr=stream if fd == 2 else subprocess.PIPE)
            assert result.returncode == 1 and bfile.read_bytes() == protected
    old_summary = summary.read_bytes()
    for fd in [1, 2]:
        with summary.open("r+") as stream:
            result = subprocess.run([str(BINARY), "--ram", "--no-bfile", "--terms", "10"],
                                    cwd=directory, text=True, timeout=30,
                                    stdout=stream if fd == 1 else subprocess.PIPE,
                                    stderr=stream if fd == 2 else subprocess.PIPE)
        assert result.returncode == 1 and summary.read_bytes() == old_summary

    # Combined terminal output still has clean result rows; only final JSON is saved.
    combined = directory / "combined.log"
    with combined.open("w") as stream:
        result = subprocess.run([str(BINARY), "--ram", "--no-bfile", "--terms", "100",
                                 "--report", "10"], cwd=directory, stdout=stream,
                                stderr=subprocess.STDOUT, timeout=30)
    assert result.returncode == 0
    lines = combined.read_text().splitlines(keepends=True)
    assert summary.read_text() == lines[-1]
    assert all(len(line.split()) == 2 for line in lines if line[:1].isdigit())

    # Inject partial-write and fsync errors only into regular files. With --ram
    # and --no-bfile, the summary temporary file is the only regular data writer.
    source = (ROOT / "346216_02.cpp").read_text()
    split = source.index("using U =")
    wrappers = r'''
static ssize_t fail_summary_write(int fd, const void* data, size_t count) {
    const char* mode = std::getenv("ULAM_SUMMARY_FAIL_MODE");
    struct stat st{};
    static bool partial = false;
    if (mode && std::string(mode) == "write" && !fstat(fd, &st) && S_ISREG(st.st_mode)) {
        if (!partial) { partial = true; return ::write(fd, data, std::min(count, size_t{2})); }
        errno = ENOSPC; return -1;
    }
    return ::write(fd, data, count);
}
static int fail_summary_fsync(int fd) {
    const char* mode = std::getenv("ULAM_SUMMARY_FAIL_MODE");
    struct stat st{};
    if (mode && std::string(mode) == "fsync" && !fstat(fd, &st) && S_ISREG(st.st_mode)) {
        errno = EIO; return -1;
    }
    if (mode && std::string(mode) == "directory_fsync" && !fstat(fd, &st) && S_ISDIR(st.st_mode)) {
        errno = EIO; return -1;
    }
    return ::fsync(fd);
}
#define write fail_summary_write
#define fsync fail_summary_fsync
'''
    injected = directory / "injected.cpp"
    injected.write_text(source[:split] + wrappers + source[split:])
    compiler = (["/usr/bin/xcrun", "--sdk", "macosx", "clang++"]
                if sys.platform == "darwin" else ["c++"])
    injected_binary = directory / "injected"
    subprocess.run([*compiler, "-std=c++17", "-O1", "-fsanitize=undefined",
                    "-fno-sanitize-recover=all", str(injected), "-o", str(injected_binary)], check=True)
    previous = summary.read_bytes()
    for mode in ["write", "fsync"]:
        env = dict(os.environ, ULAM_SUMMARY_FAIL_MODE=mode)
        result = subprocess.run([str(injected_binary), "--ram", "--no-bfile", "--terms", "100",
                                 "--report", "0"], cwd=directory, env=env,
                                capture_output=True, text=True, timeout=30)
        assert result.returncode == 1 and terminal_report(result)["status"] == "incomplete"
        assert "summary save failed" in result.stderr
        assert summary.read_bytes() == previous and not list(directory.glob("*.tmp.*"))
    result = subprocess.run([str(injected_binary), "--ram", "--no-bfile", "--terms", "100",
                             "--report", "0"], cwd=directory,
                            env=dict(os.environ, ULAM_SUMMARY_FAIL_MODE="directory_fsync"),
                            capture_output=True, text=True, timeout=30)
    assert result.returncode == 1 and terminal_report(result)["status"] == "incomplete"
    assert summary.read_text() == result.stderr.splitlines(keepends=True)[-1]

print("PASS: final JSON equals saved summary; independent residue-outlier counts and forced exceptions;")
print("default/custom filenames, latest-file replacement, no-summary, resource stop, setup failure, SIGTERM;")
print("b-file/cache/lock/stream aliases, hardlinks, symlink refusal, writer lock, combined terminal output;")
print("partial summary writes and fsync failure preserve the previous file and return incomplete.")
print("directory fsync failure publishes an incomplete report and returns failure.")
