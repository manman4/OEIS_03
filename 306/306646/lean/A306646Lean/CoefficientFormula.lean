import A306646Lean.Definitions

/-!
# Exactness of the coefficient formula

This file proves that the natural-number division in every summand is exact.
The key binomial identity is

`j * choose (j - 1) (r - 1) = r * choose j r`.
-/

namespace A306646Lean

private theorem index_mul_le {n k j : ℕ} (hk : 0 < k) (hj : j ≤ n / k) :
    k * j ≤ n := by
  have h : j * k ≤ n := (Nat.le_div_iff_mul_le hk).mp hj
  simpa [Nat.mul_comm] using h

private theorem index_dvd_remainder_choose {j r : ℕ} (hj : 1 ≤ j) :
    j ∣ r * Nat.choose j r := by
  cases r with
  | zero => simp
  | succ r =>
      have hj_eq : j - 1 + 1 = j := Nat.sub_add_cancel hj
      have h := Nat.add_one_mul_choose_eq (j - 1) r
      rw [hj_eq] at h
      refine ⟨Nat.choose (j - 1) r, ?_⟩
      simpa [Nat.mul_comm] using h.symm

/-- Every denominator occurring in the coefficient formula divides its numerator. -/
theorem index_dvd_n_mul_choose {n k j : ℕ} (hk : 0 < k) (hj0 : 1 ≤ j)
    (hj : j ≤ n / k) :
    j ∣ n * Nat.choose j (n - k * j) := by
  let r := n - k * j
  have hkj : k * j ≤ n := index_mul_le hk hj
  have hn : k * j + r = n := by
    simpa [r] using Nat.add_sub_of_le hkj
  have hfirst : j ∣ (k * j) * Nat.choose j r := by
    refine ⟨k * Nat.choose j r, ?_⟩
    simp [Nat.mul_assoc, Nat.mul_left_comm]
  have hsecond : j ∣ r * Nat.choose j r := index_dvd_remainder_choose hj0
  change j ∣ n * Nat.choose j r
  rw [← hn, Nat.add_mul]
  exact Nat.dvd_add hfirst hsecond

/-- Multiplying a summand by its index recovers the numerator exactly. -/
theorem summand_mul_index {n k j : ℕ} (hk : 0 < k) (hj0 : 1 ≤ j)
    (hj : j ≤ n / k) :
    summand n k j * j = n * Nat.choose j (n - k * j) := by
  unfold summand
  exact Nat.div_mul_cancel (index_dvd_n_mul_choose hk hj0 hj)

end A306646Lean
