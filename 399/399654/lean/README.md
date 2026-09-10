# A399654Lean

This Lean 4 project formalizes the edge bound proved in
[`../A399654_EDGE_BOUND_PROOF_JA.md`](../A399654_EDGE_BOUND_PROOF_JA.md) for
[OEIS A399654](https://oeis.org/A399654).

The abstract part treats cycles as finite subsets of an arbitrary finite edge
set.  It proves that every realizable finite set `S` of cycle lengths has an
edge-minimal realization `H`, that one selected cycle of each length in `S`
covers `H`, and consequently that

```text
|H| <= sum_{L in S} L.
```

The final module instantiates this set-system theorem for Mathlib finite simple
graphs.  There a cycle is represented by the finite set of edges of a closed
simple walk, and the abstract notion of realization is proved equivalent to
the usual statement that the graph has exactly the cycle lengths in `S`.

## Formal results

- `exists_edgeMinimal_subset` constructs an edge-minimal sub-realization.
- `exists_length_all_cycles_contain_edge` proves that deleting any edge of a
  minimal realization destroys every cycle of at least one target length.
- `subset_biUnion_selected_cycles` proves that one selected cycle of each
  target length covers every edge of a minimal realization.
- `edgeMinimal_card_le_sum` derives the cardinality bound for a minimal
  realization, and `exists_realization_card_le_sum` gives the abstract
  existence theorem.
- `exists_spanning_subgraph_card_le_sum` states the concrete graph result: a
  finite simple graph realizing `S` has a spanning subgraph that realizes the
  same `S` and has at most `sum_{L in S} L` edges.

## Reproducible build

Enter this `lean` directory, then run:

```sh
lake update
lake exe cache get
lake build
lake env lean A399654Lean/AxiomAudit.lean
```

Lean and Mathlib are pinned to `v4.27.0`, and `lake-manifest.json` records the
exact dependency commits.  The project contains no `sorry` or custom axioms.
All project paths are relative, so this directory can be moved as a unit and
built under a different parent directory.

The formalization was AI-assisted and should be reviewed as source code; Lean
checks the resulting proof terms independently.
