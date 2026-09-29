import A374275Lean.GoldenFactorTransport

/-!
# The least nontrivial split norm: 11

The element `τ = 1 + 2α` has norm 11.  It is irreducible, is not
associated to its conjugate, and its associate class occurs in the
factorization of a rational integer `k` exactly when `11 ∣ k`.
-/

namespace A374275Lean

open QuadraticAlgebra

namespace GoldenRing

/-- `τ = 1 + 2α`, a factor of 11. -/
def tau : R := beta 1 2

@[simp] lemma norm_tau : QuadraticAlgebra.norm tau = 11 := by
  norm_num [tau, norm_beta, form]

lemma irreducible_of_norm_natPrime {z : R} {p : ℕ} (hp : p.Prime)
    (hz : QuadraticAlgebra.norm z = (p : ℤ)) : Irreducible z := by
  rw [irreducible_iff]
  constructor
  · intro hunit
    have hnunit : IsUnit (QuadraticAlgebra.norm z) :=
      (QuadraticAlgebra.isUnit_iff_norm_isUnit.mp hunit)
    have hpOne : p = 1 := by
      have : ((p : ℤ).natAbs) = 1 := by
        apply Int.isUnit_iff_natAbs_eq.mp
        simpa [hz] using hnunit
      simpa using this
    exact hp.ne_one hpOne
  · intro a b hab
    have hnorm : QuadraticAlgebra.norm a * QuadraticAlgebra.norm b = (p : ℤ) := by
      rw [← map_mul, ← hab, hz]
    have habs : (QuadraticAlgebra.norm a).natAbs *
        (QuadraticAlgebra.norm b).natAbs = p := by
      rw [← Int.natAbs_mul, hnorm]
      simp
    have hone : (QuadraticAlgebra.norm a).natAbs = 1 ∨
        (QuadraticAlgebra.norm b).natAbs = 1 := by
      by_contra h
      push_neg at h
      exact (Nat.not_prime_of_mul_eq habs h.1 h.2) hp
    rcases hone with ha | hb
    · left
      apply QuadraticAlgebra.isUnit_iff_norm_isUnit.mpr
      exact Int.isUnit_iff_natAbs_eq.mpr ha
    · right
      apply QuadraticAlgebra.isUnit_iff_norm_isUnit.mpr
      exact Int.isUnit_iff_natAbs_eq.mpr hb

lemma tau_irreducible : Irreducible tau :=
  irreducible_of_norm_natPrime (by norm_num) norm_tau

/-- The irreducible associate class represented by `τ`. -/
def tauClass : IrredClass :=
  ⟨Associates.mk tau, Associates.irreducible_mk.mpr tau_irreducible⟩

@[simp] lemma tauClass_val : (tauClass : Associates R) = Associates.mk tau := rfl

lemma tauClass_ne_conj : tauClass ≠ conjIrred tauClass := by
  intro h
  have hclasses : Associates.mk (beta 1 2) = Associates.mk (beta 2 1) := by
    have hv := congrArg Subtype.val h
    change Associates.mk (beta 1 2) = conjAssoc (Associates.mk (beta 1 2)) at hv
    rw [conjAssoc_mk_beta] at hv
    exact hv
  have h12 : (1, 2) ∈ positiveSolutions 11 := by
    apply mem_positiveSolutions_iff.mpr
    norm_num [form]
  have h21 : (2, 1) ∈ positiveSolutions 11 := by
    apply mem_positiveSolutions_iff.mpr
    norm_num [form]
  have hpairs : (1, 2) = (2, 1) :=
    classOfBeta_injective_on (k := 11) (x₁ := (1, 2)) (x₂ := (2, 1))
      h12 h21 hclasses
  have := congrArg Prod.fst hpairs
  norm_num at this

lemma tauClass_not_conj_eq : conjIrred tauClass ≠ tauClass :=
  Ne.symm tauClass_ne_conj

lemma tau_dvd_natCast_iff (k : ℕ) : tau ∣ (k : R) ↔ 11 ∣ k := by
  constructor
  · rintro ⟨z, hz⟩
    have hnorm := congrArg QuadraticAlgebra.norm hz
    rw [map_mul, norm_tau] at hnorm
    have hdivInt : (11 : ℤ) ∣ (k : ℤ) ^ 2 := by
      refine ⟨QuadraticAlgebra.norm z, ?_⟩
      simpa [pow_two] using hnorm
    have hdivNat : 11 ∣ k ^ 2 := by
      exact_mod_cast hdivInt
    exact (by norm_num : Nat.Prime 11).dvd_of_dvd_pow hdivNat
  · rintro ⟨m, rfl⟩
    refine ⟨star tau * (m : R), ?_⟩
    apply QuadraticAlgebra.ext <;> simp [tau, beta] <;> ring

lemma tauClass_mem_factors_natCast_iff {k : ℕ} (hk : 0 < k) :
    (tauClass : Associates R) ∈ (Associates.mk (k : R)).factors ↔ 11 ∣ k := by
  constructor
  · intro hmem
    have hdvd : (tauClass : Associates R) ∣ Associates.mk (k : R) :=
      Associates.dvd_of_mem_factors hmem
    rw [tauClass_val, Associates.mk_dvd_mk, tau_dvd_natCast_iff] at hdvd
    exact hdvd
  · intro hdiv
    have hkR : (k : R) ≠ 0 := by
      intro hkzero
      have := congrArg QuadraticAlgebra.re hkzero
      simp at this
      omega
    simpa [tauClass] using
      (Associates.mem_factors_of_dvd hkR tau_irreducible
        ((tau_dvd_natCast_iff k).2 hdiv))

/-- Up to conjugation, `τ` is the only non-self-conjugate reduced class of
positive norm at most 11. -/
lemma small_reduced_class {z : R} (hzRed : IsReduced z)
    (hzPos : 0 < QuadraticAlgebra.norm z)
    (hzLe : QuadraticAlgebra.norm z ≤ 11) :
    Associates.mk z = (tauClass : Associates R) ∨
      Associates.mk z = (conjIrred tauClass : IrredClass).1 ∨
      conjAssoc (Associates.mk z) = Associates.mk z := by
  let p := pairOfReduced z
  have hzBeta : beta p.1 p.2 = z := beta_pairOfReduced hzRed
  have hpPos : 0 < p.1 := pairOfReduced_pos hzRed
  have hnorm : (form p.1 p.2 : ℤ) = QuadraticAlgebra.norm z := by
    rw [← norm_beta, hzBeta]
  have hpForm : form p.1 p.2 ≤ 11 := by exact_mod_cast hnorm.trans_le hzLe
  have hxLe : p.1 ≤ 3 := by
    rw [form] at hpForm
    nlinarith
  have hyLe : p.2 ≤ 2 := by
    rw [form] at hpForm
    by_contra h
    have : 3 ≤ p.2 := by omega
    nlinarith
  have hpCases : p = (1, 0) ∨ p = (1, 1) ∨ p = (1, 2) ∨
      p = (2, 0) ∨ p = (2, 1) ∨ p = (3, 0) := by
    rcases p with ⟨x, y⟩
    simp only at hpPos hxLe hyLe hpForm ⊢
    interval_cases x
    all_goals interval_cases y
    all_goals norm_num [form] at hpForm
    all_goals norm_num
  rcases hpCases with hp | hp | hp | hp | hp | hp
  · rw [hp] at hzBeta
    exact Or.inr (Or.inr (by rw [← hzBeta]; simp [beta]))
  · rw [hp] at hzBeta
    exact Or.inr (Or.inr (by rw [← hzBeta, conjAssoc_mk_beta]))
  · rw [hp] at hzBeta
    exact Or.inl (by rw [← hzBeta]; rfl)
  · rw [hp] at hzBeta
    exact Or.inr (Or.inr (by rw [← hzBeta]; simp [beta]))
  · rw [hp] at hzBeta
    exact Or.inr (Or.inl (by
      rw [← hzBeta]
      change Associates.mk (beta 2 1) = conjAssoc (Associates.mk (beta 1 2))
      rw [conjAssoc_mk_beta]))
  · rw [hp] at hzBeta
    exact Or.inr (Or.inr (by rw [← hzBeta]; simp [beta]))

end GoldenRing
end A374275Lean
