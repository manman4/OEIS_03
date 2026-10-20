"""Quick C regression checks; no third-party Python packages are required."""

import argparse
import math
import os
from pathlib import Path
import random
import shlex
import subprocess
import tempfile


def run(command, **kwargs):
    return subprocess.run(command, text=True, capture_output=True, timeout=30, **kwargs)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ubsan", action="store_true")
    args = parser.parse_args()
    root = Path(__file__).resolve().parent
    compiler = shlex.split(os.environ.get("CC", "cc"))
    flags = ["-std=c11", "-O2", "-Wall", "-Wextra", "-Wconversion",
             "-Wshadow", "-pedantic", "-Werror"]
    if args.ubsan:
        flags += ["-fsanitize=undefined", "-fno-sanitize-recover=all"]

    with tempfile.TemporaryDirectory(prefix="a306477-test-") as directory:
        work = Path(directory)
        binary = str(work / "a306477")
        subprocess.run(compiler + flags + [str(root / "a306477.c"), "-lm", "-o", binary],
                       check=True)

        bound = 30
        terms = [[math.comb(v + shift, k) for v in range(bound + 1)]
                 for shift, k in [(2, 2), (3, 4), (5, 6), (7, 8)]]
        expected = [0] * (bound + 1)
        # Direct definition: all 31^4 tuples, with no square test or residue filter.
        for a in terms[0]:
            for b in terms[1]:
                for c in terms[2]:
                    for d in terms[3]:
                        total = a + b + c + d
                        if total <= bound:
                            expected[total] += 1
        result = run([binary] + list(map(str, range(bound + 1))), check=True)
        assert result.stdout.splitlines() == [f"a({n}) = {c}" for n, c in enumerate(expected)]
        assert not result.stderr

        invalid = ["", "-1", "-0", "+1", " 1", "1 ", "1a", "1.0", "1e3", "abc",
                   "1000000000000001", "2305843009213693952", "18446744073709551615",
                   "18446744073709551616", "9" * 100]
        for value in invalid:
            result = run([binary, value])
            assert result.returncode != 0 and not result.stdout and result.stderr, value
        result = run([binary, "1000000000000000", "bad"])
        assert result.returncode != 0 and not result.stdout and result.stderr
        result = run([binary, "00001"], check=True)
        assert result.stdout == "a(1) = 1\n" and not result.stderr
        with open(os.devnull, "rb") as readonly_stdout:
            result = subprocess.run([binary, "1"], stdout=readonly_stdout,
                                    stderr=subprocess.PIPE, text=True, timeout=30)
        assert result.returncode == 1 and "failed to write results" in result.stderr

        fixture = work / "fixture.c"
        fixture.write_text(r'''
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned allocation_count, fail_at;
static void *test_malloc(size_t n) {
    if (++allocation_count == fail_at) { errno = ENOMEM; return NULL; }
    return malloc(n);
}
static void *test_realloc(void *p, size_t n) {
    if (++allocation_count == fail_at) { errno = ENOMEM; return NULL; }
    return realloc(p, n);
}
#define malloc test_malloc
#define realloc test_realloc
#define main original_main
#include "a306477.c"
#undef main
int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "parse") == 0) {
        u64 n = 0;
        if (!parse_n("1000000000000000", &n) || n != max_n) return 1;
        return 0;
    }
    if (argc == 2 && strcmp(argv[1], "tri") == 0) {
        u64 n;
        while (scanf("%" SCNu64, &n) == 1) printf("%d\n", is_tri(n));
        return 0;
    }
    if (argc == 2 && strcmp(argv[1], "malloc") == 0) fail_at = 1;
    else if (argc == 2 && strcmp(argv[1], "realloc") == 0) fail_at = 2;
    else return 2;
    (void)count(max_n);
    return 0;
}
''')
        helper = str(work / "fixture")
        subprocess.run(compiler + flags + ["-I", str(root), str(fixture), "-lm", "-o", helper],
                       check=True)
        result = run([helper, "parse"], check=True)
        assert not result.stdout and not result.stderr

        rng = random.Random(306477)
        samples = {0, 1, 2, 2**61 - 1, 2**61, 2**61 + 1, 2**64 - 1, 10**15}
        samples.update(rng.randrange(2**64) for _ in range(1000))
        for w in [2, 3, 65536, 2**31, 2**32, 6074001000]:
            triangular = w * (w - 1) // 2
            samples.update(n for n in [triangular - 1, triangular, triangular + 1]
                           if 0 <= n < 2**64)
        samples = sorted(samples)
        result = run([helper, "tri"], input="\n".join(map(str, samples)) + "\n", check=True)
        expected_tri = [int(n > 0 and math.isqrt(8*n + 1)**2 == 8*n + 1) for n in samples]
        assert list(map(int, result.stdout.split())) == expected_tri
        assert not result.stderr
        for mode in ["malloc", "realloc"]:
            result = run([helper, mode])
            assert result.returncode == 1 and not result.stdout and mode in result.stderr

    print(f"PASS: n=0..{bound} matches direct four-variable enumeration.")
    print("PASS: invalid inputs rejected; upper limit accepted; all arguments checked first.")
    print(f"PASS: {len(samples)} triangular tests, including the former 2^61 overflow case.")
    print("PASS: malloc/realloc failures reported on stderr with exit status 1.")
    print("PASS: stdout write failure reported with exit status 1.")


if __name__ == "__main__":
    main()
