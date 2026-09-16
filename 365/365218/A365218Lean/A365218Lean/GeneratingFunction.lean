import A365218Lean.Definitions
import A365218Lean.LagrangeInversion

/-!
# The generating function of A365218

This file connects the original implicit generating-function equation to the
coefficient formula used in `Definitions.lean`.
-/

noncomputable section

namespace A365218Lean

open Polynomial
open scoped PowerSeries

/-- The original implicit generating-function equation from OEIS A365218. -/
def IsA365218GeneratingFunction (A : ℚ⟦X⟧) : Prop :=
  A = 1 + (PowerSeries.X * A ^ 6) *
    (1 + PowerSeries.X * A ^ 6)⁻¹

private theorem denominator_constantCoeff (A : ℚ⟦X⟧) :
    PowerSeries.constantCoeff (1 + PowerSeries.X * A ^ 6) = 1 := by
  simp

private theorem denominator_ne_zero (A : ℚ⟦X⟧) :
    1 + PowerSeries.X * A ^ 6 ≠ 0 := by
  intro h
  have := congrArg PowerSeries.constantCoeff h
  rw [denominator_constantCoeff] at this
  norm_num at this

private theorem inverse_denominator_mul (A : ℚ⟦X⟧) :
    ((PowerSeries.X * A ^ 6) * (1 + PowerSeries.X * A ^ 6)⁻¹) *
        (1 + PowerSeries.X * A ^ 6) =
      PowerSeries.X * A ^ 6 := by
  calc
    ((PowerSeries.X * A ^ 6) * (1 + PowerSeries.X * A ^ 6)⁻¹) *
          (1 + PowerSeries.X * A ^ 6) =
        (PowerSeries.X * A ^ 6) *
          ((1 + PowerSeries.X * A ^ 6)⁻¹ *
            (1 + PowerSeries.X * A ^ 6)) := by ring
    _ = PowerSeries.X * A ^ 6 := by
      rw [PowerSeries.inv_mul_cancel _
        (by rw [denominator_constantCoeff]; norm_num), mul_one]

/-- Subtracting one converts the OEIS equation to its Lagrange form. -/
theorem generatingFunction_sub_one_fixedPoint {A : ℚ⟦X⟧}
    (hA : IsA365218GeneratingFunction A) :
    A - 1 = PowerSeries.X *
      Polynomial.aeval (A - 1) coefficientKernel := by
  have hquot :
      A - 1 = (PowerSeries.X * A ^ 6) *
        (1 + PowerSeries.X * A ^ 6)⁻¹ := by
    change A = 1 + (PowerSeries.X * A ^ 6) *
      (1 + PowerSeries.X * A ^ 6)⁻¹ at hA
    calc
      A - 1 =
          (1 + (PowerSeries.X * A ^ 6) *
            (1 + PowerSeries.X * A ^ 6)⁻¹) - 1 :=
        congrArg (fun F : ℚ⟦X⟧ => F - 1) hA
      _ = _ := by ring
  have hmul := congrArg (fun F : ℚ⟦X⟧ =>
    F * (1 + PowerSeries.X * A ^ 6)) hquot
  change (A - 1) * (1 + PowerSeries.X * A ^ 6) =
    ((PowerSeries.X * A ^ 6) * (1 + PowerSeries.X * A ^ 6)⁻¹) *
      (1 + PowerSeries.X * A ^ 6) at hmul
  rw [inverse_denominator_mul] at hmul
  have hpoly :
      Polynomial.aeval (A - 1) coefficientKernel =
        (1 - (A - 1)) * (1 + (A - 1)) ^ 6 := by
    simp [coefficientKernel]
  rw [hpoly]
  have hAeq : 1 + (A - 1) = A := by ring
  rw [hAeq]
  linear_combination hmul

/-- The coefficient of every solution of the OEIS equation is the formula in `a365218`. -/
theorem generatingFunction_coeff_eq_a365218 {A : ℚ⟦X⟧}
    (hA : IsA365218GeneratingFunction A) (n : ℕ) :
    PowerSeries.coeff n A = a365218 n := by
  cases n with
  | zero =>
      change A = 1 + (PowerSeries.X * A ^ 6) *
        (1 + PowerSeries.X * A ^ 6)⁻¹ at hA
      have h := congrArg PowerSeries.constantCoeff hA
      simp at h
      simpa [a365218, PowerSeries.coeff_zero_eq_constantCoeff_apply] using h
  | succ n =>
      let B : ℚ⟦X⟧ := A - 1
      have hfix : B = PowerSeries.X * Polynomial.aeval B coefficientKernel := by
        exact generatingFunction_sub_one_fixedPoint hA
      have hkernel : coefficientKernel.coeff 0 = 1 := by
        rw [Polynomial.coeff_zero_eq_eval_zero]
        simp [coefficientKernel]
      have hlagrange := lagrange_inversion_coeff coefficientKernel hkernel B
        hfix (n + 1) (by omega)
      have hcoeff : PowerSeries.coeff (n + 1) A =
          PowerSeries.coeff (n + 1) B := by
        simp [B]
      rw [hcoeff, hlagrange]
      simp [a365218, auxiliaryCoeff, coefficientPolynomial]

end A365218Lean
