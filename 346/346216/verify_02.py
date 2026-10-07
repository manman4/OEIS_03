"""Definition-based checks, forced heuristic failures, and durable b-file tests."""
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


def invoke(*args, success=True, reason=None, cwd=ROOT):
    result = subprocess.run([str(BINARY), "--no-summary", *map(str, args)], cwd=cwd,
                            capture_output=True, text=True, timeout=90)
    assert "runtime error:" not in result.stderr, result.stderr
    assert "UndefinedBehaviorSanitizer" not in result.stderr, result.stderr
    assert (result.returncode == 0) == success, result.stderr
    if reason:
        assert reason in result.stderr, result.stderr
    if not success:
        assert '"status":"complete"' not in result.stderr
        return result
    return result, json.loads(result.stderr.splitlines()[-1])


def oracle(count, limit=200000):
    # No residues, compression, recent window, threshold, or known OEIS values.
    counts = bytearray(limit + 1)
    terms = [1, 2]
    counts[3] = 1
    candidate = 3
    while len(terms) < count:
        assert candidate <= limit
        if counts[candidate] == 1:
            for a in terms:
                s = a + candidate
                if s > limit:
                    break
                if counts[s] < 2:
                    counts[s] += 1
            terms.append(candidate)
        candidate += 1
    digest = 14695981039346656037
    for value in terms:
        digest = ((digest ^ value) * 1099511628211) % (1 << 64)
    rows = "".join(f"{i} {terms[10 ** i - 1]}\n" for i in range(len(str(count))))
    return terms[-1], digest, rows


value, digest, expected = oracle(10000)
base = ["--terms", 10000, "--verify-until", 120000, "--report", 0]
first, stats = invoke("--ram", "--no-bfile", *base)
assert first.stdout == expected
assert stats["value"] == value and stats["digest64"] == digest
assert stats["verified_candidates_through"] == 120000
self_test = subprocess.run([str(BINARY), "--ram", "--self-test"],
                           capture_output=True, text=True, timeout=30)
assert self_test.returncode == 0 and "self_test=passed" in self_test.stderr

cases = [
    ["--p", 22, "--q", 9],
    ["--p", 120500181, "--q", 49315733],
    ["--p", 2, "--q", 1],
    ["--p", 3, "--q", 1],
    ["--threshold", 0],
    ["--window", 1],
    ["--anchor-limit", 0],
    ["--anchor-limit", 4],
    ["--dictionary-limit", 1],
    ["--threshold", 0, "--window", 1, "--anchor-limit", 4, "--dictionary-limit", 1],
]
for flags in cases:
    result, s = invoke("--ram", "--no-bfile", *base, *flags)
    assert result.stdout == expected and s["digest64"] == digest
    if "--dictionary-limit" in flags:
        assert s["escaped_blocks"] > 0
    if any(f in flags for f in ["--threshold", "--window", "--anchor-limit"]):
        assert s["fallback_calls"] > 0

with tempfile.TemporaryDirectory(prefix="ulam02-verify-") as directory:
    directory = Path(directory)
    disk = directory / "table.bin"
    result, s = invoke("--disk", disk, "--cache-mib", 1, "--no-bfile", *base,
                       "--dictionary-limit", 1, "--threshold", 0, "--window", 1)
    assert result.stdout == expected and s["digest64"] == digest and s["escaped_blocks"] > 0
    before = disk.read_bytes()
    invoke("--disk", disk, "--no-bfile", *base, success=False, reason="cannot create new disk file")
    assert disk.read_bytes() == before

    address = directory / "address.bin"
    r = subprocess.run([str(BINARY), "--disk", str(address), "--self-test"],
                       capture_output=True, text=True, timeout=30)
    assert r.returncode == 0 and "self_test=passed" in r.stderr

    # A small dictionary forces raw escapes across more than 1 MiB of disk pages.
    million = ["--terms", 1000000, "--report", 0, "--no-bfile"]
    ram_m, sm = invoke("--ram", *million)
    disk_m, sd = invoke("--disk", directory / "million.bin", "--cache-mib", 1,
                        "--dictionary-limit", 1, *million)
    assert ram_m.stdout == disk_m.stdout and sm["digest64"] == sd["digest64"]
    assert sd["escaped_blocks"] > 100000

    bfile = directory / "b346216.txt"
    r, s = invoke("--ram", "--bfile", bfile, *base, "--report", 1000)
    assert r.stdout == expected and bfile.read_text() == expected
    progress = [line for line in r.stderr.splitlines() if line.startswith("progress ")]
    assert len(progress) == 10 and "percent=100.00" in progress[-1]
    original = bfile.read_bytes()
    # Same target rechecks existing lines without appending duplicates.
    invoke("--ram", "--bfile", bfile, *base)
    assert bfile.read_bytes() == original

    # A failed stdout write cannot be reported as complete; the b-file row
    # that preceded the failed stdout write remains a complete persisted row.
    if Path("/dev/full").exists():
        failed_output = directory / "failed-output.txt"
        with open("/dev/full", "w") as stream:
            r = subprocess.run([str(BINARY), "--no-summary", "--ram", "--terms", "100",
                                "--bfile", str(failed_output), "--report", "0"],
                               stdout=stream, stderr=subprocess.PIPE, text=True, timeout=30)
        assert r.returncode == 1 and "stdout write failed" in r.stderr
        assert failed_output.read_text() == "0 1\n"

    # Shorter calculation followed by a longer one: replay and append only new rows.
    shorter = directory / "shorter.txt"
    invoke("--ram", "--bfile", shorter, "--terms", 100, "--report", 0)
    invoke("--ram", "--bfile", shorter, *base)
    assert shorter.read_text() == expected

    with open(bfile, "r+") as locked:
        fcntl.flock(locked, fcntl.LOCK_EX | fcntl.LOCK_NB)
        invoke("--ram", "--bfile", bfile, *base, success=False, reason="locked")
    assert bfile.read_bytes() == original

    for text, reason in [
        ("0 2\n", "disagrees"),
        ("0 1\n2 690\n", "inconsistent"),
        ("0 1\n1 18", "incomplete final line"),
        ("0 1\n1 18\n1 18\n", "inconsistent"),
        ("0 -1\n", "invalid unsigned integer"),
    ]:
        broken = directory / "broken.txt"
        broken.write_text(text)
        invoke("--ram", "--bfile", broken, *base, success=False, reason=reason)
        assert broken.read_text() == text
    link = directory / "link.txt"
    link.symlink_to(bfile)
    invoke("--ram", "--bfile", link, *base, success=False, reason="open b-file")
    assert bfile.read_bytes() == original

    # SIGTERM preserves complete rows, releases the lock, and permits a replay.
    partial = directory / "partial.txt"
    log = directory / "signal.log"
    with open(log, "w") as stderr, open(os.devnull, "w") as stdout:
        process = subprocess.Popen([str(BINARY), "--no-summary", "--ram", "--terms", "10000000",
                                    "--bfile", str(partial), "--report", "0",
                                    "--window", "1", "--anchor-limit", "0"],
                                   stdout=stdout, stderr=stderr)
        deadline = time.monotonic() + 10
        while time.monotonic() < deadline:
            if partial.exists() and b"1 18\n" in partial.read_bytes():
                break
            if process.poll() is not None:
                raise AssertionError("signal test exited before SIGTERM")
            time.sleep(0.002)
        process.send_signal(signal.SIGTERM)
        assert process.wait(timeout=10) == 1
    assert "interrupted by signal" in log.read_text()
    assert partial.read_bytes().endswith(b"\n")
    invoke("--ram", "--bfile", partial, *base)
    assert partial.read_text() == expected

    # stdout and stderr share an fd: result rows must stay on their own lines.
    combined = directory / "combined.log"
    with open(combined, "w") as stream:
        r = subprocess.run([str(BINARY), "--no-summary", "--ram", "--terms", "10000", "--no-bfile",
                            "--report", "1000"], stdout=stream, stderr=subprocess.STDOUT, timeout=30)
    assert r.returncode == 0
    text = combined.read_text()
    assert "\r" not in text
    rows = [line for line in text.splitlines() if line[:1].isdigit()]
    assert "\n".join(rows) + "\n" == expected

    # Default output name is tested only in a temporary directory.
    r, s = invoke("--ram", *base, cwd=directory)
    assert (directory / "b346216.txt").read_text() == expected

invoke("--ram", "--no-bfile", "--terms", 1, "--report", 0)
invoke("--ram", "--no-bfile", "--terms", 2, "--report", 0)
invoke("--ram", "--no-bfile", "--terms", 10000, "--max-value", 100,
       success=False, reason="max-value reached")
invoke("--ram", "--no-bfile", "--terms", 10000000, "--memory-mib", 1,
       success=False, reason="RAM tables exceed")
invoke("--ram", "--no-bfile", "--terms", 10000000, "--seconds", 1, "--report", 0,
       success=False, reason="time cap reached")
for flags in [["--terms", 0], ["--terms", "-1"], ["--q", 0], ["--window", 0],
              ["--dictionary-limit", 256], ["--max-value", 2000000000001],
              ["--terms", "18446744073709551616"], ["--cache-mib", 0]]:
    invoke("--ram", "--no-bfile", *flags, success=False)

print("PASS: independent Python pair-sum oracle and all candidate decisions through 120,000;")
print("five ratios, deliberately insufficient anchors/window/dictionary, >256 synthetic patterns;")
print("RAM/disk and raw-escape equality, >2^32 addressing, cache eviction, existing-file protection;")
print("b-file order, replay, duplicate prevention, writer lock, truncated/wrong rows, SIGTERM;")
print("stdout/stderr separation, default filename, resource/time/argument limits.")
