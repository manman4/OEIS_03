import A330432Lean.Permutations

/-!
# The A330432 product theorem
-/

namespace A330432Lean

/-- For every natural number `n`, the number of permutations `sigma` of
`[1, ..., n]` satisfying `n <= k * sigma(k)` at every position `k` is

`prod k in Icc 1 n, (n - k + 2 - ceil(n / k))`.

Natural-number subtraction is used; the proof shows that every factor is the
cardinality of the corresponding set of available values. -/
theorem a330432Permutation_card_eq_product (n : ℕ) :
    Fintype.card (A330432Permutation n) = a330432 n := by
  calc
    Fintype.card (A330432Permutation n) =
        Fintype.card (A330432Choice n) :=
      Fintype.card_congr (choiceEquivA330432Permutation n).symm
    _ = a330432 n := card_a330432Choice_eq_product n

end A330432Lean
