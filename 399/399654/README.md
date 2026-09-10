# Erdős problem #84: small exact enumeration

This directory contains two deliberately independent computations of the small
values associated with [Erdős problem #84](https://www.erdosproblems.com/84).

An unofficial Japanese translation of the problem statement is available in
[`PROBLEM_JA.md`](PROBLEM_JA.md).

The edge bound shared by the candidate-spectrum searches is proved in
[`A399654_EDGE_BOUND_PROOF_JA.md`](A399654_EDGE_BOUND_PROOF_JA.md).  Its Lean 4
formalization is in [`lean/`](lean/).

For a simple undirected graph `G` on `n` vertices, its cycle set is

```text
{k in {3, ..., n} : G contains a simple cycle of length k}.
```

The desired value `f(n)` is the number of different cycle sets realized by
graphs on `n` vertices. The empty cycle set is included because forests are
graphs on `n` vertices.

## The two computations

### A. All labeled graphs (`enumerate_labeled.py`)

This program enumerates all `2^(n choose 2)` labeled simple graphs. It first
constructs every possible cycle as an edge mask. It then uses a subset (zeta)
propagation over graph masks: if a graph contains a cycle edge mask, every
supergraph contains that cycle too.

The practical limit is intentionally fixed at `n <= 7`.

### B. All non-isomorphic graphs (`enumerate_unlabeled.py`)

This program uses NetworkX's Graph Atlas, which contains all non-isomorphic
simple graphs with at most seven vertices. For each graph it checks candidate
vertex orderings directly to determine which simple cycle lengths occur.

This method differs from A in both respects that matter:

- it enumerates non-isomorphic graphs rather than all labeled graphs;
- it detects cycles by direct vertex-order checks rather than edge-mask subset
  propagation.

Agreement therefore provides a useful independent check, although it is not a
formal proof that both programs are bug-free.

### C. Non-isomorphic graphs beyond seven vertices (`enumerate_nauty.py`)

NetworkX's Graph Atlas stops at seven vertices. This program instead streams
all non-isomorphic graphs produced by nauty's `geng`, decodes graph6 records
locally, and detects cycle lengths with a minimum-rooted DFS. It is intended
first for `n=8`, where there are 12,346 non-isomorphic graphs.

The exact values through `n=10` were already reported in Alvin Dunås's 2026
Uppsala thesis, *The number of sets of cycle lengths for graphs on n vertices*.
This computation should therefore be described as an independent reproduction,
not as a new table. In particular, the reference value at `n=8` is `f(8)=40`.

## Suggested commands

Install the extra dependency in an isolated environment:

```bash
python3 -m venv .venv
source .venv/bin/activate
python -m pip install -r experiments/problem_84/requirements.txt
```

Start with a small range:

```bash
python experiments/problem_84/enumerate_labeled.py --max-n 5
python experiments/problem_84/enumerate_unlabeled.py --max-n 5
python experiments/problem_84/compare_results.py
```

Run the independent verification checks:

```bash
python experiments/problem_84/verify_problem_84.py
```

Then increase the range. Method A grows as `2^(n choose 2)`, so `n=7` is much
more expensive than `n=6`:

```bash
python experiments/problem_84/enumerate_labeled.py --max-n 7
python experiments/problem_84/enumerate_unlabeled.py --max-n 7
python experiments/problem_84/compare_results.py
```

To extend the non-isomorphic enumeration to `n=8`, first install nauty. With
Homebrew on macOS:

```bash
brew install nauty
```

Then generate `n=3,...,8`. Including the overlap is intentional: it checks the
new generator and detector against both earlier methods through `n=7`.

```bash
/usr/bin/time -p python experiments/problem_84/enumerate_nauty.py --max-n 8
python experiments/problem_84/verify_nauty_results.py
```

The script looks for both `geng` (Homebrew) and `nauty-geng` (common on Debian).
An explicit path can be supplied when necessary:

```bash
python experiments/problem_84/enumerate_nauty.py \
  --max-n 8 --geng /path/to/geng
```

Do not run `enumerate_labeled.py` with `n=8`: it would require considering
268,435,456 labeled graphs, and that implementation deliberately refuses
values above seven.

By default the scripts write `results_labeled.json` and
`results_unlabeled.json` in this directory. Each realized cycle set has a
witness graph stored as an edge list.

The nauty extension writes `results_nauty.json` and stores one graph6 witness
for each realized cycle set.

### D. Optimized exhaustive enumeration (`enumerate_nauty_fast.cpp`)

This is a C++20 port of the graph6 decoder and minimum-rooted DFS used by
`enumerate_nauty.py`. It still streams every non-isomorphic graph from `geng`,
but represents cycle sets as integer bit masks and avoids Python object overhead
inside the DFS. It checks the complete `geng` graph count for every requested
order before writing a result.

The C++ detector is deliberately checked by the existing Python verifier. The
verifier decodes every stored witness both locally and with NetworkX, recomputes
its cycle set with the Python DFS, and compares all available orders with the
two earlier enumerations.

From the repository root, build and run it as follows:

```bash
c++ -std=c++20 -O3 -Wall -Wextra -Wpedantic \
  experiments/problem_84/enumerate_nauty_fast.cpp \
  -o /tmp/enumerate_nauty_fast

/usr/bin/time -p /tmp/enumerate_nauty_fast \
  --max-n 11 \
  --geng /opt/homebrew/bin/geng \
  --output /tmp/results_nauty_fast.json

python experiments/problem_84/verify_nauty_results.py \
  --input /tmp/results_nauty_fast.json
```

On systems where `geng` is already on `PATH`, the `--geng` option can be
omitted. The default output name is `results_nauty_fast.json` in the current
working directory.

For `n=11`, `geng` emits 1,018,997,864 non-isomorphic graphs, about 85 times
the count for `n=10`. The program reports progress after every ten million
graphs. To compute only the new order, use `--min-n 11 --max-n 11`; omitting
`--min-n` also repeats the smaller orders and preserves all overlap checks.
The verifier has no reference value for `f(11)`: it checks the complete graph
count and every stored witness, but labels the resulting `f(11)` as having no
reference value.

