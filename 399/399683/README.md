# OEIS A399683: path anti-Ramsey triangle

This directory contains exact computations for
[OEIS A399683](https://oeis.org/A399683), the triangular array

```text
T(n, k) = AR(n, P_k),  3 <= k <= n,
```

read by rows in increasing `n`, and within each row in increasing `k`.
Here `P_k` is the path on `k` vertices and `AR(n, P_k)` is the maximum
number of colors in an edge-coloring of `K_n` containing no rainbow copy of
`P_k`.

The shared statement of
[Erdős problem #1105](https://www.erdosproblems.com/1105) is translated in
[`../PROBLEM_JA.md`](../PROBLEM_JA.md).  An elementary proof of the cases
`k = 3, 4`, which lie outside Yuan's `k >= 5` theorem, is recorded in
[`PATH_SMALL_CASES_JA.md`](PATH_SMALL_CASES_JA.md).  Its Lean 4
formalization is in [`lean/`](lean/).

## Naive computation

`enumerate_paths_bruteforce.py` does not use the published formula.  It
treats an edge-coloring as a partition of `E(K_n)` into nonempty color
classes and enumerates every such partition exactly once, up to renaming the
colors.  It then tests every labeled copy of every path `P_k` directly.
Consequently, a stored coloring proves each lower bound, while exhaustive
enumeration of all partitions proves the corresponding upper bound.

The number of partitions of an `m`-element set is the Bell number `B_m`:

```text
n = 5: m = 10, B_m = 115975
n = 6: m = 15, B_m = 1382958545
```

The Python script therefore deliberately refuses `n > 5`.  This is a small,
independent check of the definitions and initial values, not the eventual
fast implementation.

From the repository root, first enter this sequence directory:

```bash
cd experiments/problem_1105/A399683
```

Run the computation there.  Use a separate rerun file so that the stored
reference result is not overwritten:

```bash
/usr/bin/time -p python enumerate_paths_bruteforce.py \
  --max-n 5 \
  --output results_paths_bruteforce_rerun.json
```

The default output is `results_paths_bruteforce.json` in this directory.  It
includes the flattened triangle and one witness coloring for each pair
`(n, k)`.  Existing output is not replaced unless `--force` is supplied; use
a different `--output` filename for a reproducibility rerun.

Verify the saved counts, formula values, flattened ordering, and witnesses
with a separate rainbow-path detector:

```bash
python verify_paths_bruteforce.py \
  --input results_paths_bruteforce_rerun.json
```

The verifier does not import the enumerator.  It computes Bell numbers from
Stirling numbers of the second kind and searches for rainbow paths by a
vertex-by-vertex depth-first search, rather than using precomputed path
permutations.

The calculation is expected to agree with Long-Tu Yuan's exact formula for
`n >= k >= 5`.  The cases `k = 3, 4` are included so that the OEIS
triangle uses the natural full range `n >= k >= 3`.

## Formula-free C search

`enumerate_paths_c.c` uses the same mathematical search space but is not a
line-by-line port of the Python program.  It assigns colors recursively and
checks a path as soon as all of its edges have been assigned.  A subtree is
discarded only if every relevant `k` has already acquired a rainbow path,
or if even giving every remaining edge a new color cannot improve the best
coloring already found.  Neither the search nor its initial bounds use the
published anti-Ramsey formula.

Compile with warnings enabled and place the disposable binary outside the
repository:

```bash
cc -O3 -std=c11 -Wall -Wextra -Wpedantic \
  enumerate_paths_c.c \
  -o /tmp/enumerate_paths_c
```

First check the overlap with the Python computation, again writing a separate
rerun file:

```bash
/usr/bin/time -p /tmp/enumerate_paths_c \
  --max-n 5 \
  --output results_paths_c_rerun.json

python verify_paths_bruteforce.py --input results_paths_c_rerun.json
```

Then compute rows beyond the Python limit separately:

```bash
/usr/bin/time -p /tmp/enumerate_paths_c \
  --min-n 6 --max-n 6 \
  --output results_paths_c_n6_rerun.json

/usr/bin/time -p /tmp/enumerate_paths_c \
  --min-n 7 --max-n 7 \
  --output results_paths_c_n7_rerun.json
```

Progress is printed every ten million search nodes by default.  Use
`--progress-every N` to change the interval.  Ctrl-C or `SIGTERM` stops
the recursive search without writing a partial result file.  The program also
checks for an existing output file before starting a potentially long search.

The same independent witness and formula checker also accepts a stored C
output directly:

```bash
python verify_paths_bruteforce.py --input results_paths_c.json
```

Here `terminal_colorings` counts only leaves reached after pruning, not all
Bell-number colorings.  Every omitted subtree has either an already completed
rainbow path for each still-relevant `k`, or an upper bound no better than
the incumbent recorded by the search.

The C search is still exponential.  Its `n <= 7` limit is intentional;
raising the constant without another complexity review is not supported.
