# OEIS A399687: cycle anti-Ramsey triangle

This directory contains exact computations for
[OEIS A399687](https://oeis.org/A399687), the triangular array

```text
T(n, k) = AR(n, C_k),  3 <= k <= n,
```

read by rows in increasing `n`, and within each row in increasing `k`.
Here `C_k` is the cycle on `k` vertices and `AR(n, C_k)` is the maximum
number of colors in an edge-coloring of `K_n` containing no rainbow copy of
`C_k`.

The shared statement of
[Erdős problem #1105](https://www.erdosproblems.com/1105) is translated in
[`../PROBLEM_JA.md`](../PROBLEM_JA.md).

The published result is stated using the minimum number of colors that forces
a rainbow cycle; this is one more than the maximum number of colors avoiding
one used here.

## Naive computation

`enumerate_cycles_bruteforce.py` is formula-free.  It enumerates all
canonical set partitions of `E(K_n)` and directly tests every labeled
undirected cycle.  Rotations and reversals of a cycle are removed before the
search.  As with the naive path search, the Bell-number growth makes `n = 5`
the deliberate limit.

From the repository root, first enter this sequence directory:

```bash
cd experiments/problem_1105/A399687
```

Run the exhaustive computation there.  Use a separate rerun file so that the
stored reference result is not overwritten:

```bash
/usr/bin/time -p python enumerate_cycles_bruteforce.py \
  --max-n 5 \
  --output results_cycles_bruteforce_rerun.json
```

Then verify the number of colorings and cycles, the stored witness colorings,
the flattened row order, and the published formula using an independent DFS
cycle detector:

```bash
python verify_cycles_bruteforce.py \
  --input results_cycles_bruteforce_rerun.json
```

The expected initial rows are deliberately not hard-coded in the enumerator.
They should be obtained by exhaustive search before relying on the formula.

## Formula-free C search

`enumerate_cycles_c.c` uses restricted-growth colorings and incremental
cycle checks.  It prunes only after a completed rainbow cycle makes a target
`k` irrelevant, or when the number of remaining edges cannot improve the
current best value.  The published formula is not used for searching or
initialization.

Compile it with warnings enabled:

```bash
cc -O3 -std=c11 -Wall -Wextra -Wpedantic \
  enumerate_cycles_c.c \
  -o /tmp/enumerate_cycles_c
```

First reproduce the Python range and compare every overlapping entry, writing
a separate rerun file:

```bash
/usr/bin/time -p /tmp/enumerate_cycles_c \
  --max-n 5 \
  --output results_cycles_c_rerun.json

python verify_cycles_bruteforce.py \
  --input results_cycles_c_rerun.json \
  --reference results_cycles_bruteforce.json
```

Then compute `n = 6` and `n = 7` in separate output files:

```bash
/usr/bin/time -p /tmp/enumerate_cycles_c \
  --min-n 6 --max-n 6 \
  --output results_cycles_c_n6_rerun.json

python verify_cycles_bruteforce.py --input results_cycles_c_n6_rerun.json

/usr/bin/time -p /tmp/enumerate_cycles_c \
  --min-n 7 --max-n 7 \
  --output results_cycles_c_n7_rerun.json
```

The C search supports `n <= 7`.  Interrupting with Ctrl-C or `SIGTERM`
writes no partial JSON, and an existing output file is not replaced unless
`--force` is supplied.
