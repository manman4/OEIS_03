import A365218Lean.Definitions

/-!
# Coefficient recurrence for A365218

For fixed n, write

(1-t)^n * (1+t)^(6*n) = sum k, c_k t^k.

Differentiating gives

(1-t^2) C'(t) = n * (5-7*t) * C(t),

and hence

(k+1) * c_(k+1) = 5*n*c_k - (7*n-k+1)*c_(k-1).
-/

noncomputable section

namespace A365218Lean

open Polynomial

private theorem coefficientKernel_derivative_identity :
    (1 - X ^ 2) * derivative coefficientKernel =
      (5 - 7 * X) * coefficientKernel := by
  have h6 : C (6 : ℚ) = (6 : ℚ[X]) := by
    exact Polynomial.C_ofNat (R := ℚ) 6
  simp [coefficientKernel, derivative_mul, derivative_pow, h6]
  ring

private theorem coefficientPolynomial_derivative_identity (n : ℕ) :
    (1 - X ^ 2) * derivative (coefficientPolynomial n) =
      C (n : ℚ) * (5 - 7 * X) * coefficientPolynomial n := by
  cases n with
  | zero =>
      simp [coefficientPolynomial]
  | succ n =>
      rw [coefficientPolynomial, derivative_pow_succ]
      push_cast
      calc
        (1 - X ^ 2) *
              (C ((n : ℚ) + 1) * coefficientKernel ^ n *
                derivative coefficientKernel) =
            C ((n : ℚ) + 1) * coefficientKernel ^ n *
              ((1 - X ^ 2) * derivative coefficientKernel) := by ring
        _ = C ((n : ℚ) + 1) * coefficientKernel ^ n *
              ((5 - 7 * X) * coefficientKernel) := by
                rw [coefficientKernel_derivative_identity]
        _ = C ((n : ℚ) + 1) * (5 - 7 * X) *
              coefficientKernel ^ (n + 1) := by
                rw [pow_succ coefficientKernel n]
                ring

private theorem coefficientPolynomial_derivative_expanded (n : ℕ) :
    derivative (coefficientPolynomial n) -
        X ^ 2 * derivative (coefficientPolynomial n) =
      C (5 * (n : ℚ)) * coefficientPolynomial n -
        C (7 * (n : ℚ)) * (X * coefficientPolynomial n) := by
  calc
    derivative (coefficientPolynomial n) -
          X ^ 2 * derivative (coefficientPolynomial n) =
        (1 - X ^ 2) * derivative (coefficientPolynomial n) := by ring
    _ = C (n : ℚ) * (5 - 7 * X) * coefficientPolynomial n :=
      coefficientPolynomial_derivative_identity n
    _ = C (5 * (n : ℚ)) * coefficientPolynomial n -
          C (7 * (n : ℚ)) * (X * coefficientPolynomial n) := by
            have h5 : C (5 : ℚ) = (5 : ℚ[X]) := by
              exact Polynomial.C_ofNat (R := ℚ) 5
            have h7 : C (7 : ℚ) = (7 : ℚ[X]) := by
              exact Polynomial.C_ofNat (R := ℚ) 7
            simp only [map_mul, h5, h7]
            ring

theorem auxiliaryCoeff_zero (n : ℕ) :
    auxiliaryCoeff n 0 = 1 := by
  rw [auxiliaryCoeff, coeff_zero_eq_eval_zero]
  simp [coefficientPolynomial, coefficientKernel]

theorem auxiliaryCoeff_one (n : ℕ) :
    auxiliaryCoeff n 1 = 5 * n := by
  change (coefficientPolynomial n).coeff 1 = 5 * n
  have h := coeff_derivative (coefficientPolynomial n) 0
  norm_num at h
  rw [← h, coeff_zero_eq_eval_zero]
  simp [coefficientPolynomial, coefficientKernel, derivative_pow, derivative_mul]
  ring

theorem auxiliaryCoeff_recurrence (n k : ℕ) (hk : 1 ≤ k) :
    (k + 1 : ℚ) * auxiliaryCoeff n (k + 1) =
      5 * n * auxiliaryCoeff n k -
        (7 * (n : ℚ) - k + 1) * auxiliaryCoeff n (k - 1) := by
  obtain ⟨j, rfl⟩ := Nat.exists_eq_succ_of_ne_zero (by omega : k ≠ 0)
  cases j with
  | zero =>
      have h := congrArg (fun p : ℚ[X] => p.coeff 1)
        (coefficientPolynomial_derivative_expanded n)
      have h5 : C (5 : ℚ) = (5 : ℚ[X]) := by
        exact Polynomial.C_ofNat (R := ℚ) 5
      have h7 : C (7 : ℚ) = (7 : ℚ[X]) := by
        exact Polynomial.C_ofNat (R := ℚ) 7
      simp [auxiliaryCoeff, coeff_derivative, coeff_X_pow_mul',
        map_mul, h5, h7, mul_assoc] at h ⊢
      linarith
  | succ j =>
      have h := congrArg (fun p : ℚ[X] => p.coeff (j + 2))
        (coefficientPolynomial_derivative_expanded n)
      have h5 : C (5 : ℚ) = (5 : ℚ[X]) := by
        exact Polynomial.C_ofNat (R := ℚ) 5
      have h7 : C (7 : ℚ) = (7 : ℚ[X]) := by
        exact Polynomial.C_ofNat (R := ℚ) 7
      simp [auxiliaryCoeff, coeff_derivative, coeff_X_pow_mul',
        map_mul, h5, h7, mul_assoc] at h ⊢
      ring_nf at h ⊢
      linarith

end A365218Lean
