import Mathlib.Algebra.BigOperators.Ring.Finset
import Mathlib.Data.Nat.Choose.Basic
import Mathlib.Order.Interval.Finset.Nat

/-!
# Definitions for A306646

For `n > 0`, the OEIS coefficient formula is

`n * ∑ j = 1..⌊n / k⌋, choose j (n - k * j) / j`.

The summand below performs the division in `ℕ`.  The fact that the division is
exact for every index in the summation range is proved in
`CoefficientFormula.lean`.
-/

open scoped BigOperators

namespace A306646Lean

/-- The `j`-th summand in the coefficient formula for A306646. -/
def summand (n k j : ℕ) : ℕ :=
  (n * Nat.choose j (n - k * j)) / j

/--
The square array A306646, expressed using its finite coefficient formula.

The special value at `n = 0` is the constant coefficient `k + 1` of
`(k + 1 - x^k) / (1 - x^k - x^(k+1))`.
-/
def a306646 (n k : ℕ) : ℕ :=
  if n = 0 then k + 1
  else ∑ j ∈ Finset.Icc 1 (n / k), summand n k j

end A306646Lean
