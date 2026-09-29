import A374275Lean.GoldenEleven

/-!
# A nontrivial representation count forces a split factor

If every irreducible factor of a positive rational integer is fixed by
conjugation, its norm-class set has at most one element.  Hence a
representation count at least two forces a non-self-conjugate irreducible
factor.
-/

namespace A374275Lean
namespace GoldenRing

open Associates

lemma normClass_subsingleton_of_all_factors_fixed {k : ℕ} (hk : 0 < k)
    (hfixed : ∀ p : IrredClass,
      p.1 ∈ (Associates.mk (k : R)).factors → conjIrred p = p) :
    Subsingleton (NormClass k) := by
  classical
  have hkR : (k : R) ≠ 0 := by
    intro h
    have := congrArg QuadraticAlgebra.re h
    simp at this
    omega
  have hkA : Associates.mk (k : R) ≠ 0 := by
    rw [Associates.mk_ne_zero]
    exact hkR
  obtain ⟨sK, hsK⟩ := Associates.factors_eq_some_iff_ne_zero.mpr hkA
  constructor
  intro a b
  have factor_data : ∀ c : NormClass k,
      c.1 ≠ 0 ∧
      (conjAssoc c.1).factors = c.1.factors ∧
      c.1.factors + c.1.factors = (Associates.mk (k : R)).factors := by
    intro c
    have hcProd := (mem_normClasses_iff_mul_conj hk).mp c.2
    have hc0 : c.1 ≠ 0 := by
      intro hc
      rw [hc, zero_mul] at hcProd
      exact hkA hcProd.symm
    obtain ⟨sc, hsc⟩ := Associates.factors_eq_some_iff_ne_zero.mpr hc0
    have hcLe : c.1.factors ≤ (Associates.mk (k : R)).factors := by
      apply Associates.factors_mono
      exact ⟨conjAssoc c.1, hcProd.symm⟩
    have hscLe : sc ≤ sK := by
      rw [hsc, hsK, WithTop.coe_le_coe] at hcLe
      exact hcLe
    have hmap : sc.map conjIrred = sc := by
      apply multiset_map_eq_self_of_forall_mem
      intro p hp
      apply hfixed p
      rw [hsK, Associates.mem_factorSet_some]
      exact (show p ∈ sK from Multiset.mem_of_le hscLe hp)
    have hconj : (conjAssoc c.1).factors = c.1.factors := by
      rw [factors_conjAssoc, hsc, mapFactorSet_coe, hmap]
    refine ⟨hc0, hconj, ?_⟩
    calc
      c.1.factors + c.1.factors =
          c.1.factors + (conjAssoc c.1).factors := by rw [hconj]
      _ = (c.1 * conjAssoc c.1).factors :=
        (Associates.factors_mul c.1 (conjAssoc c.1)).symm
      _ = (Associates.mk (k : R)).factors := by rw [hcProd]
  obtain ⟨ha0, -, haDouble⟩ := factor_data a
  obtain ⟨hb0, -, hbDouble⟩ := factor_data b
  obtain ⟨sa, hsa⟩ := Associates.factors_eq_some_iff_ne_zero.mpr ha0
  obtain ⟨sb, hsb⟩ := Associates.factors_eq_some_iff_ne_zero.mpr hb0
  have hdouble : a.1.factors + a.1.factors = b.1.factors + b.1.factors :=
    haDouble.trans hbDouble.symm
  have hmultiset : sa + sa = sb + sb := by
    rw [hsa, hsb, ← Associates.FactorSet.coe_add,
      ← Associates.FactorSet.coe_add, WithTop.coe_eq_coe] at hdouble
    exact hdouble
  have hsab : sa = sb := by
    apply Multiset.ext.mpr
    intro p
    have hc := congrArg (Multiset.count p) hmultiset
    simp only [Multiset.count_add] at hc
    omega
  apply Subtype.ext
  apply Associates.eq_of_factors_eq_factors
  rw [hsa, hsb, hsab]

theorem exists_nonfixed_factor_of_two_le_count {k : ℕ} (hk : 0 < k)
    (hcount : 2 ≤ representationCount k) :
    ∃ p : IrredClass,
      p.1 ∈ (Associates.mk (k : R)).factors ∧ conjIrred p ≠ p := by
  by_contra hnone
  push_neg at hnone
  have hsubNorm : Subsingleton (NormClass k) :=
    normClass_subsingleton_of_all_factors_fixed hk fun p hp ↦
      hnone p hp
  letI : Subsingleton (NormClass k) := hsubNorm
  have hcard : Nat.card (NormClassOrbit hk) ≤ 1 := by
    exact Finite.card_le_one_iff_subsingleton.mpr inferInstance
  rw [← representationCount_eq_natCard_orbits hk] at hcard
  omega

end GoldenRing
end A374275Lean
