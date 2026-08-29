# A330432Lean

This Lean 4 project proves a product formula for
[OEIS A330432](https://oeis.org/A330432).

For every natural number `n`, A330432 counts the permutations `sigma` of
`{1, ..., n}` satisfying

```text
k * sigma(k) >= n    for every 1 <= k <= n.
```

The proved formula is

```text
a(n) = product_{k=1..n} (n - k + 2 - ceil(n / k)).
```

The empty product gives `a(0) = 1`.

## Formal result

`A330432Permutation n` is the finite type of permutations in the statement,
represented without duplication by indices into the list of all permutations
of `[1, ..., n]`.  `SatisfiesA330432` expresses the positionwise inequalities
directly with `List.Forall₂`.  The top-level theorem is

```lean
theorem a330432Permutation_card_eq_product (n : Nat) :
    Fintype.card (A330432Permutation n) = a330432 n
```

where

```lean
def a330432 (n : Nat) : Nat :=
  ∏ k ∈ Finset.Icc 1 n, (n - k + 2 - (n ⌈/⌉ k))
```

## Proof outline

The proof is valid for every `n`; it is not a bounded verification of initial
terms.

1. At position `k`, the allowed values are exactly
   `{ceil(n/k), ..., n}`.
2. These allowed sets are nested as `k` increases.
3. The `k - 1` values used at earlier positions therefore all lie in the
   current allowed set.  The number of remaining choices is consequently
   `n - k + 2 - ceil(n/k)`.
4. A general theorem counts injective choices from nested finite sets as the
   product of these remaining-choice counts.
5. The recursive choice representation is proved bijective with the ordinary
   permutations of `[1, ..., n]` satisfying `n <= k * sigma(k)`.

Both directions of the bijection are formalized.  In particular, the result
does not assume that the nested-choice representation already contains every
valid permutation.

## Reproducible build

The project pins Lean and Mathlib to `v4.27.0`; `lake-manifest.json` also
records the exact dependency commits.

```sh
lake update
lake exe cache get
lake build
lake env lean A330432Lean/AxiomAudit.lean
```

The axiom audit should report only Lean/Mathlib's standard axioms such as
`propext`, `Classical.choice`, and `Quot.sound`.  The project contains no
`sorry`, custom axioms, or finite computation used in place of the theorem.

All project paths are relative, so the `A330432Lean` directory can be moved as
a unit and built in another location.
