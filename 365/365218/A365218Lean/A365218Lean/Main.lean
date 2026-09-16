import A365218Lean.GeneratingFunction
import A365218Lean.Positivity

/-!
# A365218: main theorems

The public theorems connect the original generating-function equation to the
coefficient formula and prove positivity.
-/

noncomputable section

namespace A365218Lean

open scoped PowerSeries

/-- Every coefficient of every solution of the A365218 generating-function
equation is strictly positive. -/
theorem generatingFunction_coeff_pos {A : ℚ⟦X⟧}
    (hA : IsA365218GeneratingFunction A) (n : ℕ) :
    0 < PowerSeries.coeff n A := by
  rw [generatingFunction_coeff_eq_a365218 hA]
  exact a365218_pos n

/-- The original implicit equation has at most one formal power-series solution. -/
theorem generatingFunction_unique {A C : ℚ⟦X⟧}
    (hA : IsA365218GeneratingFunction A)
    (hC : IsA365218GeneratingFunction C) :
    A = C := by
  ext n
  rw [generatingFunction_coeff_eq_a365218 hA,
    generatingFunction_coeff_eq_a365218 hC]

end A365218Lean
