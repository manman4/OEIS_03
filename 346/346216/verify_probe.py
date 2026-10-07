"""Independent candidate comparisons and failure-path tests for the probe."""
import json
import math
from pathlib import Path
import re
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parent
BINARY = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else ROOT / "ulam_probe"


def run(*args, success=True, reason=None):
    result = subprocess.run([str(BINARY), *map(str, args)], cwd=ROOT,
                            capture_output=True, text=True, timeout=60)
    assert "runtime error:" not in result.stderr, result.stderr
    assert "SUMMARY: UndefinedBehaviorSanitizer" not in result.stderr, result.stderr
    assert (result.returncode == 0) == success, result.stderr
    if reason:
        assert reason in result.stderr, result.stderr
    if success:
        stats = json.loads(result.stderr.splitlines()[-1])
        assert stats["status"] == "complete"
        return result.stdout, stats
    assert '"status":"complete"' not in result.stderr
    return result


base = ["--terms", 10000, "--verify-until", 120000, "--report", 0]
ram_stdout, ram = run("--self-test", *base)
assert ram["verified_candidates_through"] == 120000
for p, q in [(22, 9), (120500181, 49315733)]:
    output, stats = run(*base, "--p", p, "--q", q)
    assert output == ram_stdout
    assert stats["digest64"] == ram["digest64"]

# Exercise a long time allowance without running for hours. Progress must stay
# on stderr, increase to 100%, and leave the independently checked CSV intact.
progress_run = subprocess.run(
    [str(BINARY), "--terms", "10000", "--verify-until", "120000",
     "--seconds", "86400", "--report", "1000"],
    cwd=ROOT, capture_output=True, text=True, timeout=60)
assert progress_run.returncode == 0, progress_run.stderr
assert progress_run.stdout == ram_stdout
progress_lines = progress_run.stderr.splitlines()[:-1]
assert len(progress_lines) == 10
for step, line in enumerate(progress_lines, 1):
    match = re.fullmatch(
        r"progress terms=(\d+) value=(\d+) seconds=([\d.e+-]+) "
        r"target=(\d+) percent=(\d+\.\d{2}) eta_seconds=([\d.e+-]+)", line)
    assert match, line
    assert int(match[1]) == step * 1000
    assert int(match[4]) == 10000
    assert float(match[5]) == step * 10
    eta = float(match[6])
    assert math.isfinite(eta) and eta >= 0
    if step == 10:
        assert eta == 0
progress_stats = json.loads(progress_run.stderr.splitlines()[-1])
assert progress_stats["digest64"] == ram["digest64"]

with tempfile.TemporaryDirectory(prefix="ulam-probe-") as directory:
    table = Path(directory) / "membership.bin"
    disk_stdout, disk = run("--self-test", *base, "--disk", table, "--cache-mib", 1)
    assert ram_stdout == disk_stdout
    assert disk["digest64"] == ram["digest64"]
    before = table.read_bytes()
    run(*base, "--disk", table, success=False, reason="cannot create new disk file")
    assert table.read_bytes() == before

run("--terms", 100000, "--threshold", 0, success=False, reason="omitted anchors")
run("--terms", 10000, "--window", 3, success=False, reason="window exhausted")
run("--terms", 10000000000, "--memory-mib", 1, success=False, reason="RAM table exceeds")
run("--terms", 100000, "--max-value", 100, success=False, reason="max-value reached")
run("--terms", 10000000, "--seconds", 1, "--report", 0, success=False, reason="time cap reached")
for args in [("--terms", 1), ("--terms", "-1"), ("--q", 0), ("--window", 2),
             ("--cache-mib", 0), ("--seconds", 0), ("--seconds", 86401),
             ("--terms", "18446744073709551616")]:
    run(*args, success=False)

print("PASS: all 120,000 candidate decisions match the independent pair-sum oracle;")
print("three rational moduli, RAM/disk equality, >2^32 disk addressing and eviction,")
print("existing-file protection, resource limits and exact-stop conditions checked.")
print("Long time allowance and stderr progress percentages/ETA checked.")
