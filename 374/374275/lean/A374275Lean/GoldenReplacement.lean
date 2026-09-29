import A374275Lean.GoldenNontrivialFactor
import A374275Lean.GoldenPairSwap
import A374275Lean.Descent

/-!
# The unconditional factor-replacement descent

This file turns the factor transport developed in the preceding files into
the concrete replacement step at the split prime `11`.
-/

namespace A374275Lean
namespace GoldenRing

open Associates QuadraticAlgebra

lemma conjAssoc_mk_natCast (k : ℕ) :
    conjAssoc (Associates.mk (k : R)) = Associates.mk (k : R) := by
  rw [conjAssoc_mk]
  simp

lemma conjTauClass_mem_factors_natCast_iff {k : ℕ} (hk : 0 < k) :
    (conjIrred tauClass).1 ∈ (Associates.mk (k : R)).factors ↔ 11 ∣ k := by
  constructor
  · intro h
    have hdvd : (conjIrred tauClass).1 ∣ Associates.mk (k : R) :=
      Associates.dvd_of_mem_factors h
    rcases hdvd with ⟨c, hc⟩
    have htauDvd : tauClass.1 ∣ Associates.mk (k : R) := by
      refine ⟨conjAssoc c, ?_⟩
      have hc' := congrArg conjAssoc hc
      simpa [conjAssoc_mk_natCast] using hc'
    rw [tauClass_val, Associates.mk_dvd_mk, tau_dvd_natCast_iff] at htauDvd
    exact htauDvd
  · intro h
    have hstarDvd : star tau ∣ (k : R) := by
      rcases h with ⟨m, rfl⟩
      refine ⟨tau * (m : R), ?_⟩
      apply QuadraticAlgebra.ext <;> simp [tau, beta] <;> ring
    have hstarIrred : Irreducible (star tau) := by
      apply Associates.irreducible_mk.mp
      simpa [tauClass, conjIrred, conjAssoc_mk] using
        (conjIrred tauClass).2
    simpa [tauClass, conjIrred, conjAssoc_mk] using
      (Associates.mem_factors_of_dvd (a := (k : R)) (p := star tau) (by
        intro hk0
        have hkre := congrArg QuadraticAlgebra.re hk0
        simp at hkre
        omega) hstarIrred hstarDvd)

/-- Every irreducible associate class has a reduced representative of
positive norm.  Multiplication by the norm `-1` unit `φ` changes the sign of
the norm when necessary. -/
lemma exists_reduced_positive_representative (p : IrredClass) :
    ∃ z : R, IsReduced z ∧ 0 < QuadraticAlgebra.norm z ∧
      Associates.mk z = p.1 := by
  obtain ⟨z, hz⟩ := Associates.mk_surjective p.1
  have hz0 : z ≠ 0 := by
    intro hzero
    subst z
    have : p.1 = 0 := hz.symm.trans (by simp)
    exact p.2.ne_zero this
  have hn0 : QuadraticAlgebra.norm z ≠ 0 := norm_ne_zero hz0
  by_cases hn : 0 < QuadraticAlgebra.norm z
  · obtain ⟨w, hwAssoc, hwNorm, hwRed⟩ := exists_reduced_associate hn
    refine ⟨w, hwRed, hwNorm ▸ hn, ?_⟩
    rw [← hz]
    exact Associates.mk_eq_mk_iff_associated.mpr hwAssoc
  · have hnneg : QuadraticAlgebra.norm z < 0 := lt_of_le_of_ne
        (not_lt.mp hn) hn0
    let z' : R := z * phi
    have hnpos : 0 < QuadraticAlgebra.norm z' := by
      simp only [z', map_mul, norm_phi]
      nlinarith
    obtain ⟨w, hwAssoc, hwNorm, hwRed⟩ := exists_reduced_associate hnpos
    refine ⟨w, hwRed, hwNorm ▸ hnpos, ?_⟩
    rw [← hz]
    apply Associates.mk_eq_mk_iff_associated.mpr
    exact hwAssoc.trans
      (associated_mul_unit_left z phi (Units.isUnit phiUnit))

/-- A non-self-conjugate irreducible class determines a positive integer
norm `r`; multiplying the class by its conjugate gives the class of `r`. -/
lemma exists_positive_norm_of_irred (p : IrredClass) :
    ∃ (r : ℕ) (z : R), 0 < r ∧ IsReduced z ∧
      QuadraticAlgebra.norm z = (r : ℤ) ∧ Associates.mk z = p.1 ∧
      p.1 * (conjIrred p).1 = Associates.mk (r : R) := by
  obtain ⟨z, hzRed, hzPos, hzClass⟩ := exists_reduced_positive_representative p
  let r := (QuadraticAlgebra.norm z).toNat
  have hnorm : QuadraticAlgebra.norm z = (r : ℤ) := by
    symm
    exact Int.toNat_of_nonneg hzPos.le
  have hr : 0 < r := by
    have : (0 : ℤ) < (r : ℤ) := hnorm ▸ hzPos
    exact_mod_cast this
  refine ⟨r, z, hr, hzRed, hnorm, hzClass, ?_⟩
  calc
    p.1 * (conjIrred p).1 =
        Associates.mk z * conjAssoc (Associates.mk z) := by
          simp only [conjIrred_val, ← hzClass]
    _ = Associates.mk z * Associates.mk (star z) := by rw [conjAssoc_mk]
    _ = Associates.mk (z * star z) := by rw [Associates.mk_mul_mk]
    _ = Associates.mk (r : R) := by
      apply congrArg Associates.mk
      rw [← QuadraticAlgebra.algebraMap_norm_eq_mul_star z, hnorm]
      rfl

lemma exists_large_norm_of_nonfixed_factor {k : ℕ} (hk : 0 < k)
    (h11 : ¬ 11 ∣ k) (p : IrredClass)
    (hpMem : p.1 ∈ (Associates.mk (k : R)).factors)
    (hpNonfixed : conjIrred p ≠ p) :
    ∃ (r : ℕ) (z : R), 11 < r ∧ IsReduced z ∧
      QuadraticAlgebra.norm z = (r : ℤ) ∧ Associates.mk z = p.1 ∧
      p.1 * (conjIrred p).1 = Associates.mk (r : R) := by
  obtain ⟨r, z, hr, hzRed, hzNorm, hzClass, hpair⟩ :=
    exists_positive_norm_of_irred p
  have hpTau : p ≠ tauClass := by
    intro h
    subst p
    exact h11 ((tauClass_mem_factors_natCast_iff hk).mp hpMem)
  have hpConjTau : p ≠ conjIrred tauClass := by
    intro h
    subst p
    exact h11 ((conjTauClass_mem_factors_natCast_iff hk).mp hpMem)
  have hrLarge : 11 < r := by
    by_contra hnot
    have hrLe : r ≤ 11 := by omega
    have hzLe : QuadraticAlgebra.norm z ≤ 11 := by
      rw [hzNorm]
      exact_mod_cast hrLe
    rcases small_reduced_class hzRed hzLe with
      htau | hctau | hfixed
    · apply hpTau
      apply Subtype.ext
      exact hzClass.symm.trans htau
    · apply hpConjTau
      apply Subtype.ext
      exact hzClass.symm.trans hctau
    · apply hpNonfixed
      apply Subtype.ext
      change conjAssoc p.1 = p.1
      simpa [← hzClass] using hfixed
  exact ⟨r, z, hrLarge, hzRed, hzNorm, hzClass, hpair⟩

lemma nat_dvd_of_golden_natCast_dvd {d k : ℕ} (hd : 0 < d)
    (hdiv : (d : R) ∣ (k : R)) : d ∣ k := by
  rcases hdiv with ⟨z, hz⟩
  have him := congrArg QuadraticAlgebra.im hz
  have hre := congrArg QuadraticAlgebra.re hz
  simp only [QuadraticAlgebra.im_natCast, QuadraticAlgebra.im_mul,
    QuadraticAlgebra.re_natCast, QuadraticAlgebra.re_mul, zero_mul] at him hre
  have hdim : (d : ℤ) ≠ 0 := by exact_mod_cast (ne_of_gt hd)
  have hzim : z.im = 0 := by
    have him' : (d : ℤ) * z.im = 0 := by nlinarith [him]
    exact (mul_eq_zero.mp him').resolve_left hdim
  rw [hzim, mul_zero, add_zero] at hre
  have hzre : 0 ≤ z.re := by
    by_contra hneg
    have : z.re < 0 := lt_of_not_ge hneg
    have hkNonneg : (0 : ℤ) ≤ k := by omega
    nlinarith
  refine ⟨z.re.toNat, ?_⟩
  have hzcast : ((z.re.toNat : ℕ) : ℤ) = z.re := Int.toNat_of_nonneg hzre
  have hre' : (k : ℤ) = (d : ℤ) * (z.re.toNat : ℕ) := by
    rw [hzcast]
    exact hre
  exact_mod_cast hre'

lemma transportAssoc_irred (σ : IrredClass ≃ IrredClass) (p : IrredClass) :
    transportAssoc σ p.1 = (σ p).1 := by
  apply Associates.eq_of_factors_eq_factors
  rw [factors_transportAssoc, Associates.factors_self p.2,
    mapFactorSet_coe, Multiset.map_singleton,
    Associates.factors_self (σ p).2]

lemma transportAssoc_pow (σ : IrredClass ≃ IrredClass)
    (a : Associates R) (e : ℕ) :
    transportAssoc σ (a ^ e) = transportAssoc σ a ^ e := by
  induction e with
  | zero =>
      rw [pow_zero, pow_zero]
      apply Associates.eq_of_factors_eq_factors
      rw [factors_transportAssoc, Associates.factors_one]
      simpa only using
        (mapFactorSet_coe σ (0 : Multiset IrredClass))
  | succ e ih =>
      rw [pow_succ, transportAssoc_mul, ih, pow_succ]

lemma associates_mk_pow (a : R) (e : ℕ) :
    Associates.mk (a ^ e) = Associates.mk a ^ e := by
  induction e with
  | zero => simp
  | succ e ih =>
      rw [pow_succ, ← Associates.mk_mul_mk, ih, pow_succ]

/-- Replacing all occurrences of one conjugate irreducible pair by another
pair replaces the corresponding whole power in the underlying rational
integer. -/
lemma pair_swap_transport_nat {k r s : ℕ} (hk : 0 < k) (hr : 0 < r)
    (_hs : 0 < s) (p q : IrredClass)
    (hpNonfixed : conjIrred p ≠ p) (hqNonfixed : conjIrred q ≠ q)
    (hpMem : p.1 ∈ (Associates.mk (k : R)).factors)
    (hqNotMem : q.1 ∉ (Associates.mk (k : R)).factors)
    (hcqNotMem : (conjIrred q).1 ∉ (Associates.mk (k : R)).factors)
    (hpPair : p.1 * (conjIrred p).1 = Associates.mk (r : R))
    (hqPair : q.1 * (conjIrred q).1 = Associates.mk (s : R)) :
    ∃ (e c : ℕ), 0 < e ∧ k = r ^ e * c ∧ 0 < c ∧
      transportAssoc
          (conjugatePairSwap p q hpNonfixed hqNonfixed
            (fun h ↦ hqNotMem (h ▸ hpMem))
            (fun h ↦ hcqNotMem (h ▸ hpMem)))
          (Associates.mk (k : R)) = Associates.mk (s ^ e * c : R) := by
  classical
  let hpq : p ≠ q := fun h ↦ hqNotMem (h ▸ hpMem)
  let hpcq : p ≠ conjIrred q := fun h ↦ hcqNotMem (h ▸ hpMem)
  let σ := conjugatePairSwap p q hpNonfixed hqNonfixed hpq hpcq
  have hkR : (k : R) ≠ 0 := by
    intro hzero
    have hkre := congrArg QuadraticAlgebra.re hzero
    simp at hkre
    omega
  have hkA : Associates.mk (k : R) ≠ 0 :=
    Associates.mk_ne_zero.mpr hkR
  obtain ⟨sK, hsK⟩ := Associates.factors_eq_some_iff_ne_zero.mpr hkA
  have hconj : sK.map conjIrred = sK := by
    have h := factors_conjAssoc (Associates.mk (k : R))
    rw [conjAssoc_mk_natCast, hsK, mapFactorSet_coe] at h
    exact WithTop.coe_eq_coe.mp h.symm
  let e := sK.count p
  have hePos : 0 < e := by
    apply Multiset.count_pos.mpr
    rw [hsK, Associates.mem_factorSet_some] at hpMem
    exact hpMem
  have hcountConj : sK.count (conjIrred p) = e := by
    have hcount := Multiset.count_map_eq_count' conjIrred sK
      conjIrred.injective p
    rw [hconj] at hcount
    exact hcount
  let rest := sK.filter fun x ↦ x ≠ p ∧ x ≠ conjIrred p
  have hdecomp : Multiset.replicate e p +
      Multiset.replicate e (conjIrred p) + rest = sK := by
    apply Multiset.ext.mpr
    intro x
    simp only [rest, Multiset.count_add, Multiset.count_replicate,
      Multiset.count_filter]
    by_cases hxp : x = p
    · subst x
      simp [e, hpNonfixed, Ne.symm hpNonfixed]
    by_cases hxcp : x = conjIrred p
    · subst x
      simp [e, hpNonfixed, Ne.symm hpNonfixed, hcountConj]
    simp [hxp, hxcp, Ne.symm hxp, Ne.symm hxcp]
  let restClass : Associates R :=
    Associates.FactorSet.prod
      ((rest : Multiset IrredClass) : Associates.FactorSet R)
  have hKprod : Associates.mk (k : R) =
      p.1 ^ e * (conjIrred p).1 ^ e * restClass := by
    calc
      Associates.mk (k : R) = (sK.map Subtype.val).prod := by
        have hprod := Associates.factors_prod (Associates.mk (k : R))
        rw [hsK] at hprod
        exact hprod.symm
      _ = ((Multiset.replicate e p +
          Multiset.replicate e (conjIrred p) + rest).map Subtype.val).prod := by
        rw [hdecomp]
      _ = p.1 ^ e * (conjIrred p).1 ^ e * restClass := by
        simp only [Multiset.map_add, Multiset.map_replicate,
          Multiset.prod_add, Multiset.prod_replicate, restClass,
          Associates.prod_coe]
  have hKsplit : Associates.mk (k : R) =
      Associates.mk (r : R) ^ e * restClass := by
    rw [← hpPair, mul_pow]
    exact hKprod
  have hdivAssoc : Associates.mk ((r : R) ^ e) ∣
      Associates.mk (k : R) := by
    refine ⟨restClass, ?_⟩
    rw [associates_mk_pow]
    exact hKsplit
  have hdivR : (r : R) ^ e ∣ (k : R) := by
    rw [← Associates.mk_dvd_mk]
    exact hdivAssoc
  have hdivNat : r ^ e ∣ k := by
    apply nat_dvd_of_golden_natCast_dvd (pow_pos hr e)
    simpa only [Nat.cast_pow] using hdivR
  obtain ⟨c, hkc⟩ := hdivNat
  have hcPos : 0 < c := by
    by_contra hc
    have hc0 : c = 0 := by omega
    rw [hc0, mul_zero] at hkc
    omega
  have hrestClass : restClass = Associates.mk (c : R) := by
    apply mul_left_cancel₀ (a := Associates.mk (r : R) ^ e)
    · exact pow_ne_zero e (Associates.mk_ne_zero.mpr (by
        intro hzero
        have hre := congrArg QuadraticAlgebra.re hzero
        simp at hre
        omega))
    · calc
        Associates.mk (r : R) ^ e * restClass = Associates.mk (k : R) :=
          hKsplit.symm
        _ = Associates.mk ((r ^ e * c : ℕ) : R) := by rw [hkc]
        _ = Associates.mk (r : R) ^ e * Associates.mk (c : R) := by
          rw [Nat.cast_mul, Nat.cast_pow, ← associates_mk_pow,
            Associates.mk_mul_mk]
  have hqRest : q ∉ rest := by
    intro hmem
    apply hqNotMem
    rw [hsK]
    exact (Associates.mem_factorSet_some (hp := q.2)).mpr
      (Multiset.mem_of_mem_filter hmem)
  have hcqRest : conjIrred q ∉ rest := by
    intro hmem
    apply hcqNotMem
    rw [hsK]
    exact (Associates.mem_factorSet_some (hp := (conjIrred q).2)).mpr
      (Multiset.mem_of_mem_filter hmem)
  have hrestMap : rest.map σ = rest := by
    apply multiset_map_eq_self_of_forall_mem
    intro x hx
    have hxPair := (Multiset.mem_filter.mp hx).2
    have hxq : x ≠ q := by
      intro h
      subst x
      exact hqRest hx
    have hxcq : x ≠ conjIrred q := by
      intro h
      subst x
      exact hcqRest hx
    simp only [σ, conjugatePairSwap]
    simp [hxPair.1, hxPair.2, hxq, hxcq]
  have htransportRest : transportAssoc σ restClass = restClass := by
    apply Associates.eq_of_factors_eq_factors
    dsimp only [restClass]
    rw [factors_transportAssoc, Associates.prod_factors,
      mapFactorSet_coe, hrestMap]
  have hσp : σ p = q := conjugatePairSwap_apply_p _ _ _ _ _ _
  have hσcp : σ (conjIrred p) = conjIrred q := by
    rw [conjugatePairSwap_commutes p q hpNonfixed hqNonfixed hpq hpcq,
      hσp]
  refine ⟨e, c, hePos, hkc, hcPos, ?_⟩
  change transportAssoc σ (Associates.mk (k : R)) = _
  rw [hKprod, transportAssoc_mul, transportAssoc_mul,
    transportAssoc_pow, transportAssoc_pow, transportAssoc_irred,
    transportAssoc_irred, hσp, hσcp, htransportRest, hrestClass]
  rw [← mul_pow, hqPair]
  rw [← associates_mk_pow, ← Associates.mk_mul_mk]

lemma tau_pair_product :
    tauClass.1 * (conjIrred tauClass).1 = Associates.mk (11 : R) := by
  change Associates.mk tau * conjAssoc (Associates.mk tau) = _
  rw [conjAssoc_mk, Associates.mk_mul_mk,
    ← QuadraticAlgebra.algebraMap_norm_eq_mul_star, norm_tau]
  rfl

/-- The algebraic factor replacement is unconditional: if a positive value
has at least two representations and is not divisible by `11`, replacing a
non-self-conjugate factor pair by the `11` pair gives a smaller positive
integer with exactly the same number of representations. -/
theorem elevenReplacement : A374275Lean.ElevenReplacement := by
  intro k hcount h11
  have hk : 0 < k := by
    by_contra h
    have hk0 : k = 0 := by omega
    subst k
    rw [representationCount_zero] at hcount
    omega
  obtain ⟨p, hpMem, hpNonfixed⟩ :=
    exists_nonfixed_factor_of_two_le_count hk hcount
  obtain ⟨r, z, hrLarge, hzRed, hzNorm, hzClass, hpPair⟩ :=
    exists_large_norm_of_nonfixed_factor hk h11 p hpMem hpNonfixed
  have htauNot : tauClass.1 ∉ (Associates.mk (k : R)).factors := by
    intro hmem
    exact h11 ((tauClass_mem_factors_natCast_iff hk).mp hmem)
  have hctauNot : (conjIrred tauClass).1 ∉
      (Associates.mk (k : R)).factors := by
    intro hmem
    exact h11 ((conjTauClass_mem_factors_natCast_iff hk).mp hmem)
  obtain ⟨e, c, he, hkc, hc, htransport⟩ :=
    pair_swap_transport_nat hk (by omega : 0 < r) (by norm_num : 0 < 11)
      p tauClass hpNonfixed tauClass_not_conj_eq hpMem htauNot hctauNot
      hpPair tau_pair_product
  let k' := 11 ^ e * c
  have hk' : 0 < k' := by
    dsimp only [k']
    exact Nat.mul_pos (pow_pos (by norm_num) e) hc
  have hpow : 11 ^ e < r ^ e :=
    Nat.pow_lt_pow_left hrLarge he.ne'
  have hk'lt : k' < k := by
    rw [hkc]
    exact (Nat.mul_lt_mul_right hc).2 hpow
  refine ⟨k', hk'lt, ?_⟩
  have htransport' :
      transportAssoc
          (conjugatePairSwap p tauClass hpNonfixed tauClass_not_conj_eq
            (fun h ↦ htauNot (h ▸ hpMem))
            (fun h ↦ hctauNot (h ▸ hpMem)))
          (Associates.mk (k : R)) = Associates.mk (k' : R) := by
    simpa only [k', Nat.cast_mul, Nat.cast_pow] using htransport
  have hsame := representationCount_eq_of_factor_transport
    (conjugatePairSwap_commutes p tauClass hpNonfixed tauClass_not_conj_eq
      (fun h ↦ htauNot (h ▸ hpMem))
      (fun h ↦ hctauNot (h ▸ hpMem))) hk hk' htransport'
  exact hsame.symm

end GoldenRing
end A374275Lean
