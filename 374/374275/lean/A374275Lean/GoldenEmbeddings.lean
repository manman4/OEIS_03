import A374275Lean.GoldenUnits
import Mathlib.Algebra.Order.Archimedean.Basic
import Mathlib.Data.Real.Sqrt
import Mathlib.Analysis.SpecialFunctions.Pow.Real

/-!
# The two real embeddings and the reduction interval

The two roots of `T² - T - 1` give the real embeddings of the golden
integer ring.  This file supplies the exact algebra needed to reduce a
totally positive element by powers of `α = φ²`.
-/

namespace A374275Lean
namespace GoldenRing

open QuadraticAlgebra

noncomputable def sqrtFive : ℝ := Real.sqrt 5

noncomputable def phiReal : ℝ := (1 + sqrtFive) / 2

noncomputable def phiConjReal : ℝ := (1 - sqrtFive) / 2

noncomputable def alphaReal : ℝ := phiReal ^ 2

lemma sqrtFive_sq : sqrtFive ^ 2 = 5 := by
  rw [sqrtFive, Real.sq_sqrt]
  norm_num

lemma sqrtFive_pos : 0 < sqrtFive := by
  rw [sqrtFive]
  positivity

lemma phiReal_root : phiReal * phiReal = 1 + phiReal := by
  rw [phiReal]
  field_simp
  nlinarith [sqrtFive_sq]

lemma phiConjReal_root : phiConjReal * phiConjReal = 1 + phiConjReal := by
  rw [phiConjReal]
  field_simp
  nlinarith [sqrtFive_sq]

lemma phiReal_pos : 0 < phiReal := by
  rw [phiReal]
  have hs : 0 < sqrtFive := sqrtFive_pos
  linarith

lemma phiConjReal_neg : phiConjReal < 0 := by
  rw [phiConjReal]
  have : 1 < sqrtFive := by nlinarith [sqrtFive_sq, sqrtFive_pos]
  linarith

lemma phiReal_gt_one : 1 < phiReal := by
  rw [phiReal]
  have : 1 < sqrtFive := by nlinarith [sqrtFive_sq, sqrtFive_pos]
  linarith

lemma phiReal_mul_phiConjReal : phiReal * phiConjReal = -1 := by
  rw [phiReal, phiConjReal]
  field_simp
  nlinarith [sqrtFive_sq]

lemma alphaReal_gt_one : 1 < alphaReal := by
  rw [alphaReal]
  nlinarith [phiReal_gt_one]

lemma alphaReal_pos : 0 < alphaReal := lt_trans (by norm_num) alphaReal_gt_one

/-- The embedding sending `φ` to `(1+√5)/2`. -/
noncomputable def posEmbedding : R →+* ℝ :=
  (QuadraticAlgebra.lift
    (R := ℤ) (A := ℝ) (a := (1 : ℤ)) (b := (1 : ℤ))
    ⟨phiReal, by
      simpa only [Int.cast_one, one_smul] using phiReal_root⟩).toRingHom

/-- The conjugate embedding sending `φ` to `(1-√5)/2`. -/
noncomputable def negEmbedding : R →+* ℝ :=
  (QuadraticAlgebra.lift
    (R := ℤ) (A := ℝ) (a := (1 : ℤ)) (b := (1 : ℤ))
    ⟨phiConjReal, by
      simpa only [Int.cast_one, one_smul] using phiConjReal_root⟩).toRingHom

@[simp] lemma posEmbedding_apply (z : R) :
    posEmbedding z = z.re + z.im * phiReal := by
  simp [posEmbedding, QuadraticAlgebra.lift]

@[simp] lemma negEmbedding_apply (z : R) :
    negEmbedding z = z.re + z.im * phiConjReal := by
  simp [negEmbedding, QuadraticAlgebra.lift]

lemma embeddings_mul_eq_norm (z : R) :
    posEmbedding z * negEmbedding z = QuadraticAlgebra.norm z := by
  rw [posEmbedding_apply, negEmbedding_apply, QuadraticAlgebra.norm_def]
  push_cast
  rw [phiReal, phiConjReal]
  field_simp
  nlinarith [sqrtFive_sq]

lemma posEmbedding_injective : Function.Injective posEmbedding := by
  intro z w h
  have hzero : posEmbedding (z - w) = 0 := by
    rw [map_sub, h, sub_self]
  have hnorm : QuadraticAlgebra.norm (z - w) = 0 := by
    have hprod := embeddings_mul_eq_norm (z - w)
    rw [hzero, zero_mul] at hprod
    exact_mod_cast hprod.symm
  exact sub_eq_zero.mp ((norm_eq_zero_iff (z - w)).mp hnorm)

@[simp] lemma posEmbedding_phi : posEmbedding phi = phiReal := by
  simp [phi]

@[simp] lemma negEmbedding_phi : negEmbedding phi = phiConjReal := by
  simp [phi]

@[simp] lemma posEmbedding_alpha : posEmbedding alpha = alphaReal := by
  rw [alpha_eq_phi_sq, map_pow, posEmbedding_phi, alphaReal]

lemma negEmbedding_alpha : negEmbedding alpha = alphaReal⁻¹ := by
  have hprod : alphaReal * negEmbedding alpha = 1 := by
    rw [← posEmbedding_alpha, embeddings_mul_eq_norm, norm_alpha]
    norm_num
  apply mul_left_cancel₀ (ne_of_gt alphaReal_pos)
  rw [hprod, mul_inv_cancel₀ (ne_of_gt alphaReal_pos)]

/-- Ratio of the two real embeddings. -/
noncomputable def embeddingRatio (z : R) : ℝ := posEmbedding z / negEmbedding z

lemma embeddingRatio_pos {z : R} (hp : 0 < posEmbedding z)
    (hn : 0 < negEmbedding z) : 0 < embeddingRatio z :=
  div_pos hp hn

lemma posEmbedding_alphaUnit_zpow (m : ℤ) :
    posEmbedding ((alphaUnit ^ m : Rˣ) : R) = alphaReal ^ m := by
  let f := posEmbedding.toMonoidHom
  calc
    posEmbedding ((alphaUnit ^ m : Rˣ) : R) =
        (((Units.map f) (alphaUnit ^ m) : ℝˣ) : ℝ) :=
      (Units.coe_map f (alphaUnit ^ m)).symm
    _ = (((Units.map f alphaUnit) ^ m : ℝˣ) : ℝ) := by rw [map_zpow]
    _ = (((Units.map f alphaUnit : ℝˣ) : ℝ) ^ m) :=
      Units.val_zpow_eq_zpow_val _ _
    _ = alphaReal ^ m := by
      rw [Units.coe_map, coe_alphaUnit]
      change posEmbedding alpha ^ m = alphaReal ^ m
      rw [posEmbedding_alpha]

lemma negEmbedding_alphaUnit_zpow (m : ℤ) :
    negEmbedding ((alphaUnit ^ m : Rˣ) : R) = alphaReal⁻¹ ^ m := by
  let f := negEmbedding.toMonoidHom
  calc
    negEmbedding ((alphaUnit ^ m : Rˣ) : R) =
        (((Units.map f) (alphaUnit ^ m) : ℝˣ) : ℝ) :=
      (Units.coe_map f (alphaUnit ^ m)).symm
    _ = (((Units.map f alphaUnit) ^ m : ℝˣ) : ℝ) := by rw [map_zpow]
    _ = (((Units.map f alphaUnit : ℝˣ) : ℝ) ^ m) :=
      Units.val_zpow_eq_zpow_val _ _
    _ = alphaReal⁻¹ ^ m := by
      rw [Units.coe_map, coe_alphaUnit]
      change negEmbedding alpha ^ m = alphaReal⁻¹ ^ m
      rw [negEmbedding_alpha]

lemma embeddingRatio_mul_alpha_zpow (z : R) (m : ℤ)
    (hn : negEmbedding z ≠ 0) :
    embeddingRatio (z * (alphaUnit ^ m : Rˣ)) =
      embeddingRatio z * (alphaReal ^ 2) ^ m := by
  rw [embeddingRatio, map_mul, map_mul]
  rw [posEmbedding_alphaUnit_zpow, negEmbedding_alphaUnit_zpow]
  have ha : alphaReal ≠ 0 := ne_of_gt alphaReal_pos
  have ham : alphaReal ^ m ≠ 0 := zpow_ne_zero m ha
  rw [inv_zpow]
  have hsquare : (alphaReal ^ 2) ^ m = (alphaReal ^ m) ^ 2 := by
    rw [← zpow_natCast]
    rw [← zpow_mul, mul_comm, zpow_mul]
    rfl
  rw [hsquare, embeddingRatio]
  field_simp

lemma alphaReal_sq_gt_one : 1 < alphaReal ^ 2 := by
  nlinarith [alphaReal_gt_one]

/-- A positive real number, regarded as a unit of the nonnegative reals. -/
noncomputable def positiveNNRealUnit (x : ℝ) (hx : 0 < x) : Units NNReal where
  val := ⟨x, hx.le⟩
  inv := ⟨x⁻¹, (inv_pos.mpr hx).le⟩
  val_inv := by
    apply Subtype.ext
    simp [ne_of_gt hx]
  inv_val := by
    apply Subtype.ext
    simp [ne_of_gt hx]

@[simp] lemma coe_positiveNNRealUnit (x : ℝ) (hx : 0 < x) :
    (((positiveNNRealUnit x hx : Units NNReal) : NNReal) : ℝ) = x := rfl

private lemma coe_positiveNNRealUnit_zpow (x : ℝ) (hx : 0 < x) (m : ℤ) :
    ((((positiveNNRealUnit x hx) ^ m : Units NNReal) : NNReal) : ℝ) = x ^ m := by
  rw [Units.val_zpow_eq_zpow_val]
  exact NNReal.coe_zpow _ _

private lemma coe_positiveNNRealUnit_mul_zpow
    (r b : ℝ) (hr : 0 < r) (hb : 0 < b) (m : ℤ) :
    ((((positiveNNRealUnit r hr) * (positiveNNRealUnit b hb) ^ m :
      Units NNReal) : NNReal) : ℝ) = r * b ^ m := by
  rw [Units.val_mul, NNReal.coe_mul, coe_positiveNNRealUnit,
    coe_positiveNNRealUnit_zpow]

lemma existsUnique_real_mul_zpow_Ico {r b : ℝ} (hr : 0 < r) (hb : 1 < b) :
    ∃! m : ℤ, r * b ^ m ∈ Set.Ico (1 : ℝ) b := by
  let ru : Units NNReal := positiveNNRealUnit r hr
  let bu : Units NNReal := positiveNNRealUnit b (lt_trans (by norm_num) hb)
  have hbu : (1 : Units NNReal) < bu := by
    change (1 : NNReal) < (bu : NNReal)
    exact_mod_cast hb
  obtain ⟨m, hm, huniq⟩ :=
    existsUnique_mul_zpow_mem_Ico hbu ru (1 : Units NNReal)
  refine ⟨m, ?_, ?_⟩
  · constructor
    · have h := hm.1
      change (1 : NNReal) ≤ ((ru * bu ^ m : Units NNReal) : NNReal) at h
      have hR : (1 : ℝ) ≤ ((((ru * bu ^ m : Units NNReal) : NNReal) : ℝ)) := by
        exact_mod_cast h
      simpa [ru, bu, coe_positiveNNRealUnit_mul_zpow] using hR
    · have h := hm.2
      change ((ru * bu ^ m : Units NNReal) : NNReal) <
        (((1 : Units NNReal) * bu : Units NNReal) : NNReal) at h
      have hR : ((((ru * bu ^ m : Units NNReal) : NNReal) : ℝ)) <
          (((((1 : Units NNReal) * bu : Units NNReal) : NNReal) : ℝ)) := by
        exact_mod_cast h
      simpa [ru, bu, coe_positiveNNRealUnit_mul_zpow] using hR
  · intro m' hm'
    apply huniq
    constructor
    · change (1 : NNReal) ≤ ((ru * bu ^ m' : Units NNReal) : NNReal)
      have hR : (1 : ℝ) ≤ ((((ru * bu ^ m' : Units NNReal) : NNReal) : ℝ)) := by
        simpa [ru, bu, coe_positiveNNRealUnit_mul_zpow] using hm'.1
      exact_mod_cast hR
    · change ((ru * bu ^ m' : Units NNReal) : NNReal) <
        (((1 : Units NNReal) * bu : Units NNReal) : NNReal)
      have hR : ((((ru * bu ^ m' : Units NNReal) : NNReal) : ℝ)) <
          (((((1 : Units NNReal) * bu : Units NNReal) : NNReal) : ℝ)) := by
        simpa [ru, bu, coe_positiveNNRealUnit_mul_zpow] using hm'.2
      exact_mod_cast hR

/-- Every positive ratio has a unique translate in `[1, αReal²)`. -/
theorem existsUnique_reduction_power {z : R} (hp : 0 < posEmbedding z)
    (hn : 0 < negEmbedding z) :
    ∃! m : ℤ, embeddingRatio (z * (alphaUnit ^ m : Rˣ)) ∈
      Set.Ico (1 : ℝ) (alphaReal ^ 2) := by
  have hr : 0 < embeddingRatio z := embeddingRatio_pos hp hn
  obtain ⟨m, hm, huniq⟩ :=
    existsUnique_real_mul_zpow_Ico hr alphaReal_sq_gt_one
  refine ⟨m, ?_, ?_⟩
  · simpa [embeddingRatio_mul_alpha_zpow _ _ (ne_of_gt hn)] using hm
  · intro m' hm'
    apply huniq
    simpa [embeddingRatio_mul_alpha_zpow _ _ (ne_of_gt hn)] using hm'

/-- The half-open fundamental wedge in integral coordinates. -/
def IsReduced (z : R) : Prop := 0 ≤ z.im ∧ z.im < z.re

lemma posEmbedding_sub_negEmbedding (z : R) :
    posEmbedding z - negEmbedding z = (z.im : ℝ) * sqrtFive := by
  rw [posEmbedding_apply, negEmbedding_apply, phiReal, phiConjReal]
  ring

lemma alpha_sq_mul_negEmbedding_sub_posEmbedding (z : R) :
    alphaReal ^ 2 * negEmbedding z - posEmbedding z =
      (alphaReal ^ 2 - 1) * ((z.re : ℝ) - z.im) := by
  have hp4 : phiReal ^ 4 = 3 * phiReal + 2 := by
    calc
      phiReal ^ 4 = (phiReal * phiReal) * (phiReal * phiReal) := by ring
      _ = (1 + phiReal) * (1 + phiReal) := by rw [phiReal_root]
      _ = phiReal * phiReal + 2 * phiReal + 1 := by ring
      _ = 3 * phiReal + 2 := by rw [phiReal_root]; ring
  have hsum : phiReal + phiConjReal = 1 := by
    rw [phiReal, phiConjReal]
    ring
  have hprod := phiReal_mul_phiConjReal
  rw [posEmbedding_apply, negEmbedding_apply, alphaReal]
  rw [show (phiReal ^ 2) ^ 2 = phiReal ^ 4 by ring, hp4]
  linear_combination (z.im : ℝ) * (3 * hprod + 2 * hsum)

/-- The ratio interval is exactly the coordinate wedge `0 ≤ im < re`. -/
theorem embeddingRatio_mem_Ico_iff_isReduced {z : R}
    (hn : 0 < negEmbedding z) :
    embeddingRatio z ∈ Set.Ico (1 : ℝ) (alphaReal ^ 2) ↔ IsReduced z := by
  constructor
  · rintro ⟨hlow, hupp⟩
    have hdiff_nonneg : 0 ≤ posEmbedding z - negEmbedding z := by
      have h := (le_div_iff₀ hn).mp hlow
      simpa using h
    have himR : 0 ≤ (z.im : ℝ) := by
      rw [posEmbedding_sub_negEmbedding] at hdiff_nonneg
      exact (mul_nonneg_iff_of_pos_right sqrtFive_pos).mp hdiff_nonneg
    have hupper : 0 < alphaReal ^ 2 * negEmbedding z - posEmbedding z := by
      have h := (div_lt_iff₀ hn).mp hupp
      linarith
    have hreR : 0 < (z.re : ℝ) - z.im := by
      rw [alpha_sq_mul_negEmbedding_sub_posEmbedding] at hupper
      exact (mul_pos_iff_of_pos_left (sub_pos.mpr alphaReal_sq_gt_one)).mp hupper
    constructor
    · exact_mod_cast himR
    · have : (z.im : ℝ) < z.re := by linarith
      exact_mod_cast this
  · rintro ⟨him, hre⟩
    have himR : 0 ≤ (z.im : ℝ) := by exact_mod_cast him
    have hreR : 0 < (z.re : ℝ) - z.im := by
      have : (z.im : ℝ) < z.re := by exact_mod_cast hre
      linarith
    constructor
    · apply (le_div_iff₀ hn).mpr
      have hdiff : 0 ≤ posEmbedding z - negEmbedding z := by
        rw [posEmbedding_sub_negEmbedding]
        exact (mul_nonneg_iff_of_pos_right sqrtFive_pos).mpr himR
      linarith
    · apply (div_lt_iff₀ hn).mpr
      have hdiff : 0 < alphaReal ^ 2 * negEmbedding z - posEmbedding z := by
        rw [alpha_sq_mul_negEmbedding_sub_posEmbedding]
        exact (mul_pos_iff_of_pos_left (sub_pos.mpr alphaReal_sq_gt_one)).mpr hreR
      linarith

end GoldenRing
end A374275Lean
