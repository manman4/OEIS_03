## Exact small values for Problem #84, checked by two independent enumerations

I computed the number `f(n)` of cycle sets realizable by a simple undirected
graph on `n` vertices. I included the empty cycle set, realized by any forest.

The resulting values are:

| `n` | 3 | 4 | 5 | 6 | 7 |
|---:|---:|---:|---:|---:|---:|
| `f(n)` | 2 | 4 | 6 | 11 | 21 |

I used two implementations that differ in both graph enumeration and cycle
detection:

1. **All labeled graphs + edge-mask subset propagation.** This enumerates all
   `2^(n choose 2)` labeled graphs. Every labeled cycle is represented by its
   edge mask, and a Boolean-lattice subset propagation determines all cycle
   lengths present in every graph. At `n=7` this checks 2,097,152 labeled
   graphs.
2. **All non-isomorphic graphs + direct vertex-order checks.** This uses the
   NetworkX Graph Atlas and, for each candidate vertex subset, directly checks
   possible cyclic orderings. At `n=7` this checks all 1,044 non-isomorphic
   graphs in the atlas.

The two methods agree not only on `f(n)`, but on the complete collection of
realizable cycle sets for every `n=3,...,7`. Each method also records an
explicit witness graph, as an edge list, for every realized cycle set.

For reference, the 21 cycle sets found at `n=7` are:

```text
{}
{3}
{4}
{3,4}
{5}
{3,5}
{4,5}
{3,4,5}
{6}
{4,6}
{5,6}
{3,5,6}
{3,4,5,6}
{7}
{4,5,7}
{3,4,5,7}
{3,6,7}
{3,4,6,7}
{3,5,6,7}
{4,5,6,7}
{3,4,5,6,7}
```

Environment:

- Python 3.13.5
- NetworkX 3.6.1

Timings for the run through `n=7`:

- non-isomorphic/Graph Atlas method: 1.10 s real, 0.94 s user, 0.06 s sys;
- all-labeled method: 3.28 s real, 3.25 s user, 0.02 s sys.

The separate verification script checks paths, cycle graphs, and complete
graphs, then recomputes both methods for `n=3,4,5`; it passed.

Code and full JSON output with witnesses:

- [Problem #84 computation at commit `9dce962c`](https://github.com/manman4/erdosproblems/tree/9dce962c9b5310099e683bcf7f7dec457c57ce67/experiments/problem_84)

AI disclosure: the scripts and this report draft were written with OpenAI Codex
assistance and executed locally by me. The values above were produced by the
programs, not supplied directly by a language model. I am reporting them here
for reproducible checking and am not submitting this AI-assisted material to
the OEIS.
