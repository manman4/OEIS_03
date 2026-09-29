import A374275Lean.GoldenReplacement

/-!
# Existence of every positive representation count

For `n > 0`, the value `11^(2*n-1)` has exactly `n` representations.
The proof identifies its norm classes with the choices of how many copies of
the two conjugate factors of `11` occur, then quotients by conjugation.
-/

namespace A374275Lean
namespace GoldenRing

open Associates QuadraticAlgebra

lemma mk_eleven_pow (m : ℕ) :
    Associates.mk ((11 ^ m : ℕ) : R) =
      (tauClass.1 * (conjIrred tauClass).1) ^ m := by
  rw [tau_pair_product, ← associates_mk_pow]
  congr 1
  norm_num

noncomputable def tauPowerNormClass (m i : ℕ) (hi : i ≤ m) :
    NormClass (11 ^ m) :=
  ⟨tauClass.1 ^ i * (conjIrred tauClass).1 ^ (m - i), by
    apply (mem_normClasses_iff_mul_conj (pow_pos (by norm_num) m)).mpr
    rw [map_mul, map_pow, map_pow]
    change tauClass.1 ^ i * (conjIrred tauClass).1 ^ (m - i) *
        ((conjIrred tauClass).1 ^ i * tauClass.1 ^ (m - i)) = _
    rw [mk_eleven_pow m]
    calc
      tauClass.1 ^ i * (conjIrred tauClass).1 ^ (m - i) *
          ((conjIrred tauClass).1 ^ i * tauClass.1 ^ (m - i)) =
          (tauClass.1 * (conjIrred tauClass).1) ^ (i + (m - i)) := by
            rw [mul_pow, pow_add, pow_add]
            ac_rfl
      _ = (tauClass.1 * (conjIrred tauClass).1) ^ m := by
        rw [Nat.add_sub_of_le hi]⟩

@[simp] lemma tauPowerNormClass_val (m i : ℕ) (hi : i ≤ m) :
    (tauPowerNormClass m i hi).1 =
      tauClass.1 ^ i * (conjIrred tauClass).1 ^ (m - i) := rfl

lemma conj_tauPowerNormClass (m i : ℕ) (hi : i ≤ m) :
    conjNormClass (pow_pos (by norm_num) m) (tauPowerNormClass m i hi) =
      tauPowerNormClass m (m - i) (Nat.sub_le m i) := by
  apply Subtype.ext
  simp only [conjNormClass_val, tauPowerNormClass_val, map_mul, map_pow]
  rw [Nat.sub_sub_self hi]
  have ht : conjAssoc tauClass.1 = (conjIrred tauClass).1 :=
    (conjIrred_val tauClass).symm
  have hct : conjAssoc (conjIrred tauClass).1 = tauClass.1 := by
    rw [conjIrred_val, conjAssoc_involutive]
  rw [ht, hct]
  ac_rfl

lemma factors_mk_eleven_pow (m : ℕ) :
    (Associates.mk ((11 ^ m : ℕ) : R)).factors =
      ((Multiset.replicate m tauClass +
        Multiset.replicate m (conjIrred tauClass) : Multiset IrredClass) :
          Associates.FactorSet R) := by
  apply Associates.FactorSet.unique
  rw [Associates.factors_prod, Associates.prod_coe]
  simp only [Multiset.map_add, Multiset.map_replicate,
    Multiset.prod_add, Multiset.prod_replicate]
  rw [← mul_pow, tau_pair_product, ← associates_mk_pow]
  congr 1
  norm_num

lemma factors_tauPower (m i : ℕ) (hi : i ≤ m) :
    (tauPowerNormClass m i hi).1.factors =
      ((Multiset.replicate i tauClass +
        Multiset.replicate (m - i) (conjIrred tauClass) :
          Multiset IrredClass) : Associates.FactorSet R) := by
  rw [tauPowerNormClass_val, Associates.factors_mul,
    Associates.factors_prime_pow tauClass.2,
    Associates.factors_prime_pow (conjIrred tauClass).2,
    ← Associates.FactorSet.coe_add]

lemma tauPowerNormClass_injective (m : ℕ) {i j : ℕ}
    (hi : i ≤ m) (hj : j ≤ m)
    (h : tauPowerNormClass m i hi = tauPowerNormClass m j hj) : i = j := by
  classical
  have hf := congrArg (fun a : NormClass (11 ^ m) ↦ a.1.factors) h
  change (tauPowerNormClass m i hi).1.factors =
    (tauPowerNormClass m j hj).1.factors at hf
  rw [factors_tauPower m i hi, factors_tauPower m j hj,
    WithTop.coe_eq_coe] at hf
  have hc := congrArg (Multiset.count tauClass) hf
  have hni : tauClass ∉ Multiset.replicate (m - i) (conjIrred tauClass) := by
    intro hmem
    exact tauClass_ne_conj (Multiset.mem_replicate.mp hmem).2
  have hnj : tauClass ∉ Multiset.replicate (m - j) (conjIrred tauClass) := by
    intro hmem
    exact tauClass_ne_conj (Multiset.mem_replicate.mp hmem).2
  simp only [Multiset.count_add, Multiset.count_replicate,
    Multiset.count_eq_zero_of_notMem hni,
    Multiset.count_eq_zero_of_notMem hnj, add_zero] at hc
  simpa using hc

lemma tauPowerNormClass_surjective (m : ℕ) :
    Function.Surjective (fun i : Fin (m + 1) ↦
      tauPowerNormClass m i.1 (by omega)) := by
  classical
  intro a
  have hk : 0 < 11 ^ m := pow_pos (by norm_num) m
  have hprod := (mem_normClasses_iff_mul_conj hk).mp a.2
  have ha0 : a.1 ≠ 0 := by
    intro ha
    rw [ha, zero_mul] at hprod
    have hk0 : Associates.mk ((11 ^ m : ℕ) : R) ≠ 0 := by
      apply Associates.mk_ne_zero.mpr
      exact_mod_cast (pow_ne_zero m (by norm_num : (11 : ℕ) ≠ 0))
    exact hk0 hprod.symm
  obtain ⟨sA, hsA⟩ := Associates.factors_eq_some_iff_ne_zero.mpr ha0
  have hsum : sA + sA.map conjIrred =
      Multiset.replicate m tauClass +
        Multiset.replicate m (conjIrred tauClass) := by
    have hf := congrArg Associates.factors hprod
    rw [Associates.factors_mul, factors_conjAssoc, factors_mk_eleven_pow,
      hsA, mapFactorSet_coe, ← Associates.FactorSet.coe_add,
      WithTop.coe_eq_coe] at hf
    exact hf
  let i := sA.count tauClass
  let j := sA.count (conjIrred tauClass)
  have hsupport : ∀ x ∈ sA, x = tauClass ∨ x = conjIrred tauClass := by
    intro x hx
    have hxsum : x ∈ sA + sA.map conjIrred := Multiset.mem_add.mpr (Or.inl hx)
    rw [hsum, Multiset.mem_add, Multiset.mem_replicate,
      Multiset.mem_replicate] at hxsum
    exact hxsum.elim (fun h ↦ Or.inl h.2) (fun h ↦ Or.inr h.2)
  have hdecomp : Multiset.replicate i tauClass +
      Multiset.replicate j (conjIrred tauClass) = sA := by
    apply Multiset.ext.mpr
    intro x
    simp only [Multiset.count_add, Multiset.count_replicate]
    by_cases hxt : x = tauClass
    · subst x
      simp only [if_pos, i]
      rw [if_neg (Ne.symm tauClass_ne_conj)]
      simp
    by_cases hxc : x = conjIrred tauClass
    · subst x
      rw [if_neg tauClass_ne_conj, if_pos rfl]
      simp [j]
    have hxnot : x ∉ sA := by
      intro hx
      rcases hsupport x hx with h | h <;> contradiction
    rw [if_neg (Ne.symm hxt), if_neg (Ne.symm hxc)]
    simp [Multiset.count_eq_zero_of_notMem hxnot]
  have hij : i + j = m := by
    have hlenA := congrArg Multiset.card hdecomp
    have hlenSum := congrArg Multiset.card hsum
    simp only [Multiset.card_add, Multiset.card_replicate,
      Multiset.card_map] at hlenA hlenSum
    dsimp [i, j]
    omega
  have hi : i ≤ m := by omega
  refine ⟨⟨i, by omega⟩, ?_⟩
  apply Subtype.ext
  apply Associates.eq_of_factors_eq_factors
  rw [factors_tauPower, hsA, WithTop.coe_eq_coe]
  rw [show m - i = j by omega]
  exact hdecomp

noncomputable def tauPowerEquiv (m : ℕ) :
    Fin (m + 1) ≃ NormClass (11 ^ m) :=
  Equiv.ofBijective
    (fun i : Fin (m + 1) ↦ tauPowerNormClass m i.1 (by omega))
    ⟨by
      intro i j h
      apply Fin.ext
      exact tauPowerNormClass_injective m (by omega) (by omega) h,
     tauPowerNormClass_surjective m⟩

lemma tauPowerNormClass_eq_of_index_eq (m i j : ℕ)
    (hi : i ≤ m) (hj : j ≤ m) (hij : i = j) :
    tauPowerNormClass m i hi = tauPowerNormClass m j hj := by
  subst j
  rfl

lemma eleven_pow_pos (m : ℕ) : 0 < 11 ^ m := by positivity

noncomputable def tauOrbitRep (n : ℕ) (hn : 0 < n) (i : Fin n) :
    NormClassOrbit (eleven_pow_pos (2 * n - 1)) :=
  Quotient.mk (conjSetoid (eleven_pow_pos (2 * n - 1)))
    (tauPowerNormClass (2 * n - 1) i.1 (by omega))

theorem tauOrbitRep_injective (n : ℕ) (hn : 0 < n) :
    Function.Injective (tauOrbitRep n hn) := by
  intro i j hij
  change Quotient.mk (conjSetoid (eleven_pow_pos (2 * n - 1)))
      (tauPowerNormClass (2 * n - 1) i.1 (by omega)) =
    Quotient.mk (conjSetoid (eleven_pow_pos (2 * n - 1)))
      (tauPowerNormClass (2 * n - 1) j.1 (by omega)) at hij
  have hrel := Quotient.exact hij
  change tauPowerNormClass (2 * n - 1) j.1 (by omega) =
      tauPowerNormClass (2 * n - 1) i.1 (by omega) ∨
    tauPowerNormClass (2 * n - 1) j.1 (by omega) =
      conjNormClass (eleven_pow_pos (2 * n - 1))
        (tauPowerNormClass (2 * n - 1) i.1 (by omega)) at hrel
  apply Fin.ext
  rcases hrel with hsame | hconj
  · exact (tauPowerNormClass_injective (2 * n - 1)
      (by omega) (by omega) hsame).symm
  · have hc : tauPowerNormClass (2 * n - 1) j.1 (by omega) =
        tauPowerNormClass (2 * n - 1) ((2 * n - 1) - i.1)
          (Nat.sub_le _ _) := by
      exact hconj.trans (conj_tauPowerNormClass (2 * n - 1) i.1 (by omega))
    have hindex := tauPowerNormClass_injective (2 * n - 1)
      (by omega) (Nat.sub_le _ _) hc
    omega

theorem tauOrbitRep_surjective (n : ℕ) (hn : 0 < n) :
    Function.Surjective (tauOrbitRep n hn) := by
  intro q
  let c : NormClass (11 ^ (2 * n - 1)) := Quotient.out q
  have hcq :
      (Quotient.mk (conjSetoid (eleven_pow_pos (2 * n - 1))) c :
        NormClassOrbit (eleven_pow_pos (2 * n - 1))) = q :=
    Quotient.out_eq q
  obtain ⟨t, ht⟩ := (tauPowerEquiv (2 * n - 1)).surjective c
  by_cases htn : t.1 < n
  · refine ⟨⟨t.1, htn⟩, ?_⟩
    rw [← hcq]
    apply Quotient.sound
    left
    exact ht.symm
  · have hsmall : (2 * n - 1) - t.1 < n := by omega
    refine ⟨⟨(2 * n - 1) - t.1, hsmall⟩, ?_⟩
    rw [← hcq]
    apply Quotient.sound
    right
    calc
      c = tauPowerNormClass (2 * n - 1) t.1 (by omega) := ht.symm
      _ = tauPowerNormClass (2 * n - 1)
          ((2 * n - 1) - ((2 * n - 1) - t.1)) (Nat.sub_le _ _) := by
        apply tauPowerNormClass_eq_of_index_eq
        omega
      _ = conjNormClass (eleven_pow_pos (2 * n - 1))
          (tauPowerNormClass (2 * n - 1) ((2 * n - 1) - t.1)
            (Nat.sub_le _ _)) :=
        (conj_tauPowerNormClass (2 * n - 1) ((2 * n - 1) - t.1)
          (Nat.sub_le _ _)).symm

noncomputable def tauOrbitEquiv (n : ℕ) (hn : 0 < n) :
    Fin n ≃ NormClassOrbit (eleven_pow_pos (2 * n - 1)) :=
  Equiv.ofBijective (tauOrbitRep n hn)
    ⟨tauOrbitRep_injective n hn, tauOrbitRep_surjective n hn⟩

theorem representationCount_eleven_pow (n : ℕ) (hn : 0 < n) :
    representationCount (11 ^ (2 * n - 1)) = n := by
  rw [representationCount_eq_natCard_orbits
    (eleven_pow_pos (2 * n - 1))]
  calc
    Nat.card (NormClassOrbit (eleven_pow_pos (2 * n - 1))) =
        Nat.card (Fin n) := Nat.card_congr (tauOrbitEquiv n hn).symm
    _ = n := Nat.card_fin n

theorem exists_representationCount_pos (n : ℕ) (hn : 0 < n) :
    ∃ k, representationCount k = n :=
  ⟨11 ^ (2 * n - 1), representationCount_eleven_pow n hn⟩

end GoldenRing
end A374275Lean
