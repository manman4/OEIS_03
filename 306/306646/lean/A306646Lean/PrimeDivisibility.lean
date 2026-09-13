import A306646Lean.CoefficientFormula
import Mathlib.Data.Nat.GCD.Basic
import Mathlib.Data.Nat.Prime.Basic

/-!
# Prime divisibility for A306646

For a prime `p` and a column index `k ≥ 2`, every summation index satisfies
`1 ≤ j ≤ p / k < p`.  Hence `j` is coprime to `p`.  Exactness of the
coefficient formula then lets us cancel `j`, leaving a factor `p` in every
summand.
-/

namespace A306646Lean

private theorem index_lt_prime {p k j : ℕ} (hp : Nat.Prime p) (hk : 2 ≤ k)
    (hj : j ≤ p / k) : j < p := by
  have hkpos : 0 < k := Nat.lt_of_lt_of_le Nat.zero_lt_two hk
  have hjk : j * k ≤ p := (Nat.le_div_iff_mul_le hkpos).mp hj
  have hjle : j ≤ p :=
    calc
      j = j * 1 := by simp
      _ ≤ j * k := Nat.mul_le_mul_left j (Nat.one_le_iff_ne_zero.mpr hkpos.ne')
      _ ≤ p := hjk
  exact hjle.lt_of_ne fun hjp => by
    have hpk : p * k ≤ p := by simpa [hjp] using hjk
    have hone_lt_k : 1 < k := Nat.lt_of_lt_of_le Nat.one_lt_two hk
    have hp_lt_pk : p < p * k := by
      simpa using Nat.mul_lt_mul_of_pos_left hone_lt_k hp.pos
    exact (Nat.not_lt_of_ge hpk) hp_lt_pk

private theorem prime_dvd_summand {p k j : ℕ} (hp : Nat.Prime p) (hk : 2 ≤ k)
    (hj0 : 1 ≤ j) (hj : j ≤ p / k) :
    p ∣ summand p k j := by
  let c := Nat.choose j (p - k * j)
  have hjpos : 0 < j := hj0
  have hjlt : j < p := index_lt_prime hp hk hj
  have hcop : j.Coprime p := (Nat.coprime_of_lt_prime hjpos.ne' hjlt hp).symm
  have hjdiv_pc : j ∣ p * c := by
    exact index_dvd_n_mul_choose (Nat.lt_of_lt_of_le Nat.zero_lt_two hk) hj0 hj
  have hjdiv_c : j ∣ c := (hcop.dvd_mul_left).mp hjdiv_pc
  refine ⟨c / j, ?_⟩
  simp only [summand, c]
  rw [Nat.mul_div_assoc p hjdiv_c]

/-- If `p` is prime and `k ≥ 2`, then `p` divides `A(p,k)`. -/
theorem prime_dvd_a306646 {p k : ℕ} (hp : Nat.Prime p) (hk : 2 ≤ k) :
    p ∣ a306646 p k := by
  rw [a306646, if_neg hp.ne_zero]
  apply Finset.dvd_sum
  intro j hj
  have hbounds := Finset.mem_Icc.mp hj
  exact prime_dvd_summand hp hk hbounds.1 hbounds.2

end A306646Lean
