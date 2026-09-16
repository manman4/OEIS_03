import Mathlib.Algebra.Polynomial.Derivative

/-!
# Definitions for A365218

For positive n, Lagrange inversion gives a(n) as 1/n times the coefficient of
t^(n-1) in (1-t)^n * (1+t)^(6*n).  The derivation from the original implicit
generating function is formalized in `GeneratingFunction.lean`.
-/

noncomputable section

namespace A365218Lean

open Polynomial

/-- The kernel in the Lagrange equation. -/
def coefficientKernel : ℚ[X] :=
  (1 - X) * (1 + X) ^ 6

/-- The polynomial whose coefficient occurs in the Lagrange formula. -/
def coefficientPolynomial (n : ℕ) : ℚ[X] :=
  coefficientKernel ^ n

/-- The kernel-power definition equals the product in the Lagrange formula. -/
theorem coefficientPolynomial_eq_product (n : ℕ) :
    coefficientPolynomial n = (1 - X) ^ n * (1 + X) ^ (6 * n) := by
  rw [coefficientPolynomial, coefficientKernel, mul_pow]
  rw [pow_mul]

/-- The auxiliary coefficient of t^k in (1-t)^n * (1+t)^(6*n). -/
def auxiliaryCoeff (n k : ℕ) : ℚ :=
  (coefficientPolynomial n).coeff k

/--
A365218, defined from its Lagrange coefficient formula.

The value at zero is the constant coefficient of the original generating
function.
-/
def a365218 : ℕ → ℚ
  | 0 => 1
  | n + 1 => auxiliaryCoeff (n + 1) n / (n + 1)

end A365218Lean
