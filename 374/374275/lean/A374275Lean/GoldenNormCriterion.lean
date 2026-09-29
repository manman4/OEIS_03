import A374275Lean.GoldenOrbits

/-!
# Norm classes as factor selections

For a positive natural number `k`, an associate class belongs to
`normClasses k` exactly when multiplying it by its conjugate gives the
associate class of the rational integer `k`.  This form is suited to unique
factorization arguments.
-/

namespace A374275Lean

open QuadraticAlgebra

namespace GoldenRing

lemma associated_intCast_natCast_eq_or_neg {a : ℤ} {k : ℕ} (hk : 0 < k)
    (h : Associated (a : R) (k : R)) :
    a = (k : ℤ) ∨ a = -(k : ℤ) := by
  obtain ⟨u, hu⟩ := h
  have hk0 : (k : ℤ) ≠ 0 := by exact_mod_cast (Nat.ne_of_gt hk)
  have ha0 : a ≠ 0 := by
    intro ha
    subst a
    have hre := congrArg QuadraticAlgebra.re hu
    simp at hre
    exact hk0 hre.symm
  have him := congrArg QuadraticAlgebra.im hu
  simp only [QuadraticAlgebra.im_mul, QuadraticAlgebra.re_intCast,
    QuadraticAlgebra.im_intCast, QuadraticAlgebra.im_natCast, zero_mul,
    mul_zero, add_zero] at him
  have huIm : (u : R).im = 0 := (mul_eq_zero.mp him).resolve_left ha0
  have huNorm := norm_unit_eq_one_or_neg_one u
  have huRe : (u : R).re = 1 ∨ (u : R).re = -1 := by
    rw [QuadraticAlgebra.norm_def, huIm] at huNorm
    rcases huNorm with hnorm | hnorm
    · apply mul_self_eq_one_iff.mp
      simpa using hnorm
    · nlinarith
  have hre := congrArg QuadraticAlgebra.re hu
  simp only [QuadraticAlgebra.re_mul, QuadraticAlgebra.re_intCast,
    QuadraticAlgebra.im_intCast, QuadraticAlgebra.re_natCast, one_mul,
    zero_mul, add_zero] at hre
  rcases huRe with huRe | huRe
  · left
    simpa [huRe] using hre
  · right
    rw [huRe] at hre
    simpa using congrArg Neg.neg hre

/-- Associate-class criterion for exact positive norm. -/
theorem mem_normClasses_iff_mul_conj {k : ℕ} (hk : 0 < k) {a : Associates R} :
    a ∈ normClasses k ↔
      a * conjAssoc a = Associates.mk (k : R) := by
  constructor
  · intro ha
    obtain ⟨z, rfl, hz⟩ := (mem_normClasses_iff hk).mp ha
    rw [mk_mul_conjAssoc_mk, hz]
    rfl
  · intro hprod
    obtain ⟨z, rfl⟩ := Associates.mk_surjective a
    have hmk : Associates.mk (((QuadraticAlgebra.norm z : ℤ) : R)) =
        Associates.mk (k : R) := by
      exact (mk_mul_conjAssoc_mk z).symm.trans hprod
    have hassoc : Associated (((QuadraticAlgebra.norm z : ℤ) : R)) (k : R) :=
      Associates.mk_eq_mk_iff_associated.mp hmk
    rcases associated_intCast_natCast_eq_or_neg hk hassoc with hnorm | hnorm
    · apply (mem_normClasses_iff hk).mpr
      exact ⟨z, rfl, hnorm⟩
    · apply (mem_normClasses_iff hk).mpr
      refine ⟨z * phi, ?_, ?_⟩
      · apply Associates.mk_eq_mk_iff_associated.mpr
        exact associated_mul_unit_left z phi (by
          rw [← coe_phiUnit]
          exact Units.isUnit phiUnit)
      · rw [map_mul, norm_phi, hnorm]
        ring

end GoldenRing
end A374275Lean
