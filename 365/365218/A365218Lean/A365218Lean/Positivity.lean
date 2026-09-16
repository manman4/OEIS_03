import A365218Lean.CoefficientRecurrence

/-!
# Positivity of A365218

The proof follows the ratio estimate c_k / c_(k-1) > 2*n/k for
1 ≤ k ≤ n-1. In Lean it is convenient to clear the positive denominators
and prove the equivalent product inequality.
-/

noncomputable section

namespace A365218Lean

private theorem index_quadratic_bound {n k : ℕ} (hn : 2 ≤ n)
    (hk1 : 1 ≤ k) (hkn : k ≤ n - 2) :
    (k : ℚ) * (7 * (n : ℚ) - k + 1) < 6 * (n : ℚ) ^ 2 := by
  have hnq : (2 : ℚ) ≤ n := by exact_mod_cast hn
  have hkq1 : (1 : ℚ) ≤ k := by exact_mod_cast hk1
  have hkqn : (k : ℚ) ≤ n - 2 := by
    have hsum : (k : ℚ) + 2 ≤ n := by
      exact_mod_cast (by omega : k + 2 ≤ n)
    linarith
  have hfirst : 0 ≤ (n : ℚ) - 2 - k := by linarith
  have hsecond : 0 ≤ 6 * (n : ℚ) + 3 - k := by linarith
  have hproduct :
      0 ≤ ((n : ℚ) - 2 - k) * (6 * (n : ℚ) + 3 - k) :=
    mul_nonneg hfirst hsecond
  nlinarith

theorem auxiliaryCoeff_pos_and_ratio {n k : ℕ} (hn : 2 ≤ n)
    (hk1 : 1 ≤ k) (hkn : k ≤ n - 1) :
    0 < auxiliaryCoeff n k ∧
      (2 * n : ℚ) * auxiliaryCoeff n (k - 1) <
        k * auxiliaryCoeff n k := by
  induction k with
  | zero =>
      omega
  | succ k ih =>
      by_cases hk0 : k = 0
      · subst k
        rw [auxiliaryCoeff_zero, auxiliaryCoeff_one]
        constructor <;> push_cast
        · positivity
        · have hnq : (0 : ℚ) < n := by positivity
          nlinarith
      · have hkpos : 1 ≤ k := Nat.one_le_iff_ne_zero.mpr hk0
        have hkbound : k ≤ n - 1 := by omega
        have hkbound' : k ≤ n - 2 := by omega
        have ih' := ih hkpos hkbound
        rcases ih' with ⟨hckpos, hratio⟩
        have hqpos : 0 < 7 * (n : ℚ) - k + 1 := by
          have hnq : (2 : ℚ) ≤ n := by exact_mod_cast hn
          have hkq : (k : ℚ) ≤ n - 2 := by
            have hsum : (k : ℚ) + 2 ≤ n := by
              exact_mod_cast (by omega : k + 2 ≤ n)
            linarith
          nlinarith
        have hquad := index_quadratic_bound hn hkpos hkbound'
        have hmul_ratio := mul_lt_mul_of_pos_left hratio hqpos
        have hmul_quad := mul_lt_mul_of_pos_right hquad hckpos
        have hkey :
            (7 * (n : ℚ) - k + 1) * auxiliaryCoeff n (k - 1) <
              3 * n * auxiliaryCoeff n k := by
          have hnpos : (0 : ℚ) < 2 * n := by positivity
          apply lt_of_mul_lt_mul_left ?_ hnpos.le
          nlinarith
        have hrec := auxiliaryCoeff_recurrence n k hkpos
        constructor
        · have hnqpos : (0 : ℚ) < n := by positivity
          push_cast at hrec ⊢
          nlinarith
        · push_cast at hrec ⊢
          nlinarith

/-- Every term of A365218 is strictly positive. -/
theorem a365218_pos (n : ℕ) :
    0 < a365218 n := by
  cases n with
  | zero =>
      norm_num [a365218]
  | succ n =>
      by_cases hn0 : n = 0
      · subst n
        norm_num [a365218, auxiliaryCoeff_zero]
      · have hn2 : 2 ≤ n + 1 := by omega
        have h := (auxiliaryCoeff_pos_and_ratio (n := n + 1) (k := n)
          hn2 (by omega) (by omega)).1
        rw [a365218]
        exact div_pos h (by positivity)

end A365218Lean
