# Erdős problem #84: small exact enumeration

This directory contains two deliberately independent computations of the small
values associated with [Erdős problem #84](https://www.erdosproblems.com/84).

An unofficial Japanese translation of the problem statement is available in
[`PROBLEM_JA.md`](PROBLEM_JA.md).

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

## Reporting checklist

Before posting results to upstream Issue #290:

- record the exact commands and Python/NetworkX versions;
- confirm that `compare_results.py` reports exact agreement;
- inspect representative witnesses, including the empty set and `{n}`;
- retain the JSON output so every claimed cycle set has a witness;
- disclose that the initial programs were written with AI assistance;
- do not submit AI-generated or AI-assisted material directly to the OEIS.

The computation should first be reported as reproducible evidence on the GitHub
issue. Any OEIS submission requires a genuinely independent human derivation
and must follow the project's `CONTRIBUTING.md` rules.
