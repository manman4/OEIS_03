import A374275Lean.GoldenEmbeddings

/-!
# Unique reduced representatives

Every nonzero associate class of positive norm has exactly one representative
in the half-open wedge `0 ≤ im < re`.
-/

namespace A374275Lean
namespace GoldenRing

open QuadraticAlgebra

lemma norm_alphaUnit_zpow (m : ℤ) :
    QuadraticAlgebra.norm (((alphaUnit ^ m : Rˣ) : R)) = 1 := by
  let f := (QuadraticAlgebra.norm : R →* ℤ)
  have hf : Units.map f alphaUnit = 1 := by
    apply Units.ext
    rw [Units.coe_map, coe_alphaUnit, norm_alpha]
    rfl
  calc
    QuadraticAlgebra.norm (((alphaUnit ^ m : Rˣ) : R)) =
        (((Units.map f) (alphaUnit ^ m) : ℤˣ) : ℤ) :=
      (Units.coe_map f (alphaUnit ^ m)).symm
    _ = (((Units.map f alphaUnit) ^ m : ℤˣ) : ℤ) := by rw [map_zpow]
    _ = 1 := by rw [hf]; norm_num

lemma posEmbedding_pos_of_isReduced {z : R} (hz : IsReduced z) :
    0 < posEmbedding z := by
  rw [posEmbedding_apply]
  have him : 0 ≤ (z.im : ℝ) := by exact_mod_cast hz.1
  have hre : (z.im : ℝ) < z.re := by exact_mod_cast hz.2
  have hphi := phiReal_gt_one
  nlinarith

lemma negEmbedding_pos_of_isReduced_of_norm_pos {z : R} (hz : IsReduced z)
    (hnorm : 0 < QuadraticAlgebra.norm z) : 0 < negEmbedding z := by
  have hp := posEmbedding_pos_of_isReduced hz
  have hprod := embeddings_mul_eq_norm z
  have hprodPos : 0 < posEmbedding z * negEmbedding z := by
    rw [hprod]
    exact_mod_cast hnorm
  apply pos_of_mul_pos_left (b := posEmbedding z)
  · simpa [mul_comm] using hprodPos
  · exact hp.le

/-- Every positive-norm element is associated to a reduced element of the
same norm. -/
theorem exists_reduced_associate {z : R} (hnorm : 0 < QuadraticAlgebra.norm z) :
    ∃ w : R, Associated w z ∧ QuadraticAlgebra.norm w = QuadraticAlgebra.norm z ∧
      IsReduced w := by
  have hprod := embeddings_mul_eq_norm z
  have hprodPos : 0 < posEmbedding z * negEmbedding z := by
    rw [hprod]
    exact_mod_cast hnorm
  rcases (mul_pos_iff.mp hprodPos) with hpos | hneg
  · obtain ⟨m, hm, -⟩ := existsUnique_reduction_power hpos.1 hpos.2
    let w : R := z * (alphaUnit ^ m : Rˣ)
    refine ⟨w, ?_, ?_, ?_⟩
    · have h : Associated z w := ⟨alphaUnit ^ m, rfl⟩
      exact h.symm
    · simp only [w, map_mul, norm_alphaUnit_zpow, mul_one]
    · apply (embeddingRatio_mem_Ico_iff_isReduced ?_).mp hm
      simp only [map_mul, negEmbedding_alphaUnit_zpow]
      exact mul_pos hpos.2 (zpow_pos (inv_pos.mpr alphaReal_pos) m)
  · have hpneg : 0 < posEmbedding (-z) := by
      rw [map_neg]
      exact neg_pos.mpr hneg.1
    have hnneg : 0 < negEmbedding (-z) := by
      rw [map_neg]
      exact neg_pos.mpr hneg.2
    obtain ⟨m, hm, -⟩ := existsUnique_reduction_power hpneg hnneg
    let w : R := (-z) * (alphaUnit ^ m : Rˣ)
    refine ⟨w, ?_, ?_, ?_⟩
    · have heq : z * (-(alphaUnit ^ m) : Rˣ) = w := by
        simp [w]
      have h : Associated z w := ⟨-(alphaUnit ^ m), heq⟩
      exact h.symm
    · simp only [w, map_mul, QuadraticAlgebra.norm_neg, norm_alphaUnit_zpow, mul_one]
    · apply (embeddingRatio_mem_Ico_iff_isReduced ?_).mp hm
      simp only [map_mul, negEmbedding_alphaUnit_zpow]
      exact mul_pos hnneg (zpow_pos (inv_pos.mpr alphaReal_pos) m)

/-- Two associated reduced elements of the same positive norm are equal. -/
theorem eq_of_associated_of_isReduced {z w : R}
    (hz : IsReduced z) (hw : IsReduced w)
    (hnorm : QuadraticAlgebra.norm z = QuadraticAlgebra.norm w)
    (hpos : 0 < QuadraticAlgebra.norm z) (hassoc : Associated z w) : z = w := by
  have hpz := posEmbedding_pos_of_isReduced hz
  have hnz := negEmbedding_pos_of_isReduced_of_norm_pos hz hpos
  have hpw := posEmbedding_pos_of_isReduced hw
  have hpwNorm : 0 < QuadraticAlgebra.norm w := hnorm ▸ hpos
  have hnw := negEmbedding_pos_of_isReduced_of_norm_pos hw hpwNorm
  obtain ⟨u, hu⟩ := hassoc
  have hnu : QuadraticAlgebra.norm ((u : Rˣ) : R) = 1 := by
    have hnormEq := congrArg QuadraticAlgebra.norm hu
    rw [map_mul, hnorm] at hnormEq
    apply mul_left_cancel₀ (ne_of_gt hpwNorm)
    calc
      QuadraticAlgebra.norm w * QuadraticAlgebra.norm ((u : Rˣ) : R) =
          QuadraticAlgebra.norm w := hnormEq
      _ = QuadraticAlgebra.norm w * 1 := by ring
  obtain ⟨m, hum | hum⟩ := norm_one_unit_eq_signed_alpha_zpow u hnu
  · have hzw : z * (alphaUnit ^ m : Rˣ) = w := by
      rw [← hu, hum]
    obtain ⟨t, ht, huniq⟩ := existsUnique_reduction_power hpz hnz
    have hzero : embeddingRatio (z * (alphaUnit ^ (0 : ℤ) : Rˣ)) ∈
        Set.Ico (1 : ℝ) (alphaReal ^ 2) := by
      simpa using (embeddingRatio_mem_Ico_iff_isReduced hnz).mpr hz
    have hm : embeddingRatio (z * (alphaUnit ^ m : Rˣ)) ∈
        Set.Ico (1 : ℝ) (alphaReal ^ 2) := by
      rw [hzw]
      exact (embeddingRatio_mem_Ico_iff_isReduced hnw).mpr hw
    have hm0 : m = 0 := (huniq m hm).trans (huniq 0 hzero).symm
    subst m
    simpa using hzw
  · have hzw : z * (-(alphaUnit ^ m) : Rˣ) = w := by
      rw [← hu, hum]
    have himage : posEmbedding w < 0 := by
      rw [← hzw, map_mul, Units.val_neg, map_neg, posEmbedding_alphaUnit_zpow]
      exact mul_neg_of_pos_of_neg hpz (neg_neg_of_pos (zpow_pos alphaReal_pos m))
    linarith

end GoldenRing
end A374275Lean
