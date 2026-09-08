# A399683Lean

This Lean 4 project formalizes the elementary `k = 3, 4` cases in
[`../PATH_SMALL_CASES_JA.md`](../PATH_SMALL_CASES_JA.md) for
[OEIS A399683](https://oeis.org/A399683).

For a symmetric edge-coloring of a complete graph, `HasRainbowP3` and
`HasRainbowP4` express the existence of rainbow paths on three and four
vertices.  The formalized results are the structural upper and lower bounds
corresponding to

```text
T(n,3) = 1       for n >= 3,
T(4,4) = 3,
T(n,4) = 2       for n >= 5.
```

## Formal results

- `rainbowP3_of_usesAtLeastTwo` proves that two used colors force a rainbow
  `P₃`; `oneColoring_has_no_rainbowP3` supplies the one-color construction.
- `finite_no_rainbowP4_not_usesAtLeastThree` proves the upper bound of two
  colors for every finite vertex type of cardinality at least five.
  `exceptionalEdgeColoring_usesAtLeastTwo` and
  `exceptionalEdgeColoring_has_no_rainbowP4` supply the two-color construction.
- `k4_no_rainbowP4_not_usesAtLeastFour` proves the upper bound of three colors
  on `Fin 4`.  The explicitly defined `k4MatchingColoring` colors the three
  perfect matchings differently and proves the matching lower bound.

The proof for at least five vertices follows the three cases in the Japanese
note.  The `K₄` graph fact is checked over its six explicitly indexed edges;
this finite lemma is then used in a theorem for an arbitrary color type.
Neither Yuan's formula nor the repository's enumeration results are assumed.

## Reproducible build

Enter this `lean` directory, then run:

```sh
lake update
lake exe cache get
lake build
lake env lean A399683Lean/AxiomAudit.lean
```

Lean and Mathlib are pinned to `v4.27.0`, and `lake-manifest.json` records the
exact dependency commits.  The project contains no `sorry` or custom axioms.
All project paths are relative, so this directory can be moved as a unit and
built under a different parent directory.

The formalization was AI-assisted and should be reviewed as source code; Lean
checks the resulting proof terms independently.
