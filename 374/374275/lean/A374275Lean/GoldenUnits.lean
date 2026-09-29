import A374275Lean.GoldenRing
import Mathlib.NumberTheory.Pell

/-!
# Units of the golden integer ring

This file proves that every unit of `ℤ[(1+√5)/2]` is a signed integral
power of the golden ratio.  The norm-one part is reduced to Pell's equation
for `d = 5`; Mathlib's classification of Pell solutions is then used with
the explicitly verified fundamental solution `(9,4)`.
-/

namespace A374275Lean
namespace GoldenRing

open QuadraticAlgebra

/-- `φ` as a unit; its inverse is `φ - 1`. -/
def phiUnit : Rˣ where
  val := phi
  inv := phi - 1
  val_inv := by ext <;> norm_num [phi]
  inv_val := by ext <;> norm_num [phi]

@[simp] lemma coe_phiUnit : (phiUnit : R) = phi := rfl

/-- `α = φ²` as a unit. -/
def alphaUnit : Rˣ := phiUnit ^ 2

@[simp] lemma coe_alphaUnit : (alphaUnit : R) = alpha := by
  rw [alphaUnit, Units.val_pow_eq_pow_val, coe_phiUnit, ← alpha_eq_phi_sq]

@[simp] lemma norm_phi : QuadraticAlgebra.norm phi = -1 := by rfl

@[simp] lemma norm_alpha : QuadraticAlgebra.norm alpha = 1 := by rfl

/-- A norm-one ring element, packaged as a unit using conjugation. -/
def unitOfNormOne (z : R) (hz : QuadraticAlgebra.norm z = 1) : Rˣ where
  val := z
  inv := star z
  val_inv := by
    apply QuadraticAlgebra.ext
    · simp only [QuadraticAlgebra.re_mul, QuadraticAlgebra.re_star,
        QuadraticAlgebra.im_star, QuadraticAlgebra.re_one,
        QuadraticAlgebra.norm_def] at hz ⊢
      linarith
    · simp only [QuadraticAlgebra.im_mul, QuadraticAlgebra.re_star,
        QuadraticAlgebra.im_star, QuadraticAlgebra.im_one]
      ring
  inv_val := by
    rw [mul_comm]
    apply QuadraticAlgebra.ext
    · simp only [QuadraticAlgebra.re_mul, QuadraticAlgebra.re_star,
        QuadraticAlgebra.im_star, QuadraticAlgebra.re_one,
        QuadraticAlgebra.norm_def] at hz ⊢
      linarith
    · simp only [QuadraticAlgebra.im_mul, QuadraticAlgebra.re_star,
        QuadraticAlgebra.im_star, QuadraticAlgebra.im_one]
      ring

@[simp] lemma coe_unitOfNormOne (z : R) (hz) :
    ((unitOfNormOne z hz : Rˣ) : R) = z := rfl

/-- The standard Pell solution `9² - 5·4² = 1`. -/
def pellFundamental : Pell.Solution₁ 5 :=
  Pell.Solution₁.mk 9 4 (by norm_num)

lemma pellFundamental_isFundamental : Pell.IsFundamental pellFundamental := by
  refine ⟨by norm_num [pellFundamental], by norm_num [pellFundamental], ?_⟩
  intro b hbx
  by_contra hnot
  have hxhi : b.x ≤ 8 := by
    change ¬9 ≤ b.x at hnot
    omega
  have hxlo : 2 ≤ b.x := by omega
  have hp := b.prop
  interval_cases b.x
  all_goals
    have hylo : -3 ≤ b.y := by
      by_contra h
      have : b.y ≤ -4 := by omega
      nlinarith [sq_nonneg (b.y + 4)]
    have hyhi : b.y ≤ 3 := by
      by_contra h
      have : 4 ≤ b.y := by omega
      nlinarith [sq_nonneg (b.y - 4)]
    interval_cases b.y <;> norm_num at hp

/-- Embed `ℤ[√5]` into the golden integer ring via
`x + y√5 = (x-y) + 2yφ`. -/
def pellToR : ℤ√(5 : ℤ) →+* R where
  toFun z := ⟨z.re - z.im, 2 * z.im⟩
  map_zero' := rfl
  map_one' := rfl
  map_add' x y := by ext <;> simp <;> ring
  map_mul' x y := by ext <;> simp <;> ring

@[simp] lemma pellToR_re (z : ℤ√(5 : ℤ)) : (pellToR z).re = z.re - z.im := rfl

@[simp] lemma pellToR_im (z : ℤ√(5 : ℤ)) : (pellToR z).im = 2 * z.im := rfl

lemma norm_pellToR (z : ℤ√(5 : ℤ)) :
    QuadraticAlgebra.norm (pellToR z) = Zsqrtd.norm z := by
  rcases z with ⟨x, y⟩
  simp [pellToR, QuadraticAlgebra.norm_def, Zsqrtd.norm]
  ring

/-- A Pell solution gives a unit of the golden integer ring. -/
def pellToUnits : Pell.Solution₁ 5 →* Rˣ where
  toFun z := unitOfNormOne (pellToR (z : ℤ√(5 : ℤ))) (by
    rw [norm_pellToR]
    rw [Zsqrtd.norm_def]
    simpa [Pell.Solution₁.x, Pell.Solution₁.y, pow_two, mul_assoc] using z.prop)
  map_one' := by
    apply Units.ext
    rfl
  map_mul' x y := by
    apply Units.ext
    exact map_mul pellToR (x : ℤ√(5 : ℤ)) (y : ℤ√(5 : ℤ))

@[simp] lemma coe_pellToUnits (z : Pell.Solution₁ 5) :
    ((pellToUnits z : Rˣ) : R) = pellToR (z : ℤ√(5 : ℤ)) := rfl

lemma pellToUnits_fundamental : pellToUnits pellFundamental = alphaUnit ^ 3 := by
  apply Units.ext
  ext <;>
    norm_num [pellToUnits, pellFundamental, pellToR, alphaUnit, alpha, phiUnit,
      phi, pow_succ]

/-- Convert a norm-one golden integer with even second coordinate to a Pell solution. -/
noncomputable def toPellOfEven (z : R) (hz : QuadraticAlgebra.norm z = 1)
    (heven : Even z.im) : Pell.Solution₁ 5 :=
  Pell.Solution₁.mk (z.re + heven.choose) heven.choose (by
    rw [QuadraticAlgebra.norm_def] at hz
    rw [heven.choose_spec] at hz
    nlinarith)

@[simp] lemma pellToR_toPellOfEven (z : R) (hz) (heven) :
    pellToR (toPellOfEven z hz heven : ℤ√(5 : ℤ)) = z := by
  apply QuadraticAlgebra.ext
  · change z.re + heven.choose - heven.choose = z.re
    ring
  · change 2 * heven.choose = z.im
    linarith [heven.choose_spec]

lemma im_cube_even (z : R) (hz : QuadraticAlgebra.norm z = 1) : Even (z ^ 3).im := by
  have hnorm : z.re ^ 2 + z.re * z.im - z.im ^ 2 = 1 := by
    simpa [QuadraticAlgebra.norm_def, pow_two] using hz
  have him : (z ^ 3).im = 3 * z.im + 5 * z.im ^ 3 := by
    calc
      (z ^ 3).im = 3 * z.re ^ 2 * z.im + 3 * z.re * z.im ^ 2 +
          2 * z.im ^ 3 := by
        simp [pow_succ, QuadraticAlgebra.im_mul, QuadraticAlgebra.re_mul]
        ring
      _ = 3 * z.im * (z.re ^ 2 + z.re * z.im - z.im ^ 2) +
          5 * z.im ^ 3 := by ring
      _ = 3 * z.im + 5 * z.im ^ 3 := by rw [hnorm]; ring
  rw [him]
  rcases Int.even_or_odd z.im with heven | hodd
  · have h : Even (z.im * (3 + 5 * z.im ^ 2)) := heven.mul_right _
    have heq : z.im * (3 + 5 * z.im ^ 2) = 3 * z.im + 5 * z.im ^ 3 := by ring
    rwa [heq] at h
  · have h3 : Odd (3 : ℤ) := by norm_num
    have h5 : Odd (5 : ℤ) := by norm_num
    have hcoef : Even (3 + 5 * z.im ^ 2) := h3.add_odd (h5.mul hodd.pow)
    have h : Even (z.im * (3 + 5 * z.im ^ 2)) := hcoef.mul_left _
    have heq : z.im * (3 + 5 * z.im ^ 2) = 3 * z.im + 5 * z.im ^ 3 := by ring
    rwa [heq] at h

/-- Cubing is injective on units of the golden integer ring. -/
lemma unit_cube_injective : Function.Injective (fun u : Rˣ ↦ u ^ 3) := by
  intro u v huv
  change u ^ 3 = v ^ 3 at huv
  have hroot : (u * v⁻¹) ^ 3 = 1 := by
    calc
      (u * v⁻¹) ^ 3 = u ^ 3 * (v ^ 3)⁻¹ := by rw [mul_pow, inv_pow]
      _ = 1 := by rw [huv, mul_inv_cancel]
  have hcoe := congrArg (fun w : Rˣ ↦ ((w : R))) hroot
  set w : R := ((u * v⁻¹ : Rˣ) : R) with hw
  have hw3 : w ^ 3 = 1 := by simpa [w] using hcoe
  have him : w.im = 0 := by
    have hi := congrArg QuadraticAlgebra.im hw3
    simp [pow_succ, QuadraticAlgebra.im_mul, QuadraticAlgebra.re_mul] at hi
    have hi' : w.im * (3 * w.re ^ 2 + 3 * w.re * w.im + 2 * w.im ^ 2) = 0 := by
      calc
        w.im * (3 * w.re ^ 2 + 3 * w.re * w.im + 2 * w.im ^ 2) =
            (w.re * w.re + w.im * w.im) * w.im +
              (w.re * w.im + w.im * w.re + w.im * w.im) * w.re +
              (w.re * w.im + w.im * w.re + w.im * w.im) * w.im := by ring
        _ = 0 := hi
    have hnonneg : 0 ≤ 3 * w.re ^ 2 + 3 * w.re * w.im + 2 * w.im ^ 2 := by
      nlinarith [sq_nonneg (2 * w.re + w.im), sq_nonneg w.im]
    by_contra himne
    have hpos : 0 < 3 * w.re ^ 2 + 3 * w.re * w.im + 2 * w.im ^ 2 := by
      apply lt_of_le_of_ne hnonneg
      intro hzero
      have hbSq : w.im ^ 2 = 0 := by
        nlinarith [sq_nonneg (2 * w.re + w.im), sq_nonneg w.im]
      have hb : w.im = 0 := by nlinarith [sq_nonneg w.im]
      have ha : w.re = 0 := by
        rw [hb] at hzero
        nlinarith [sq_nonneg w.re]
      have hz : w.re = 0 ∧ w.im = 0 := ⟨ha, hb⟩
      have hwzero : w = 0 := QuadraticAlgebra.ext hz.1 hz.2
      have hunit : IsUnit w := by
        rw [hw]
        exact Units.isUnit (u * v⁻¹)
      exact hunit.ne_zero hwzero
    exact himne ((mul_eq_zero.mp hi').resolve_right (ne_of_gt hpos))
  have hre : w.re = 1 := by
    have hr := congrArg QuadraticAlgebra.re hw3
    simp [pow_succ, QuadraticAlgebra.re_mul, QuadraticAlgebra.im_mul, him] at hr
    have hfactor : (w.re - 1) * (w.re ^ 2 + w.re + 1) = 0 := by
      nlinarith
    have hquad : 0 < w.re ^ 2 + w.re + 1 := by
      nlinarith [sq_nonneg (2 * w.re + 1)]
    exact sub_eq_zero.mp ((mul_eq_zero.mp hfactor).resolve_right (ne_of_gt hquad))
  have hwone : u * v⁻¹ = 1 := by
    apply Units.ext
    exact QuadraticAlgebra.ext hre him
  exact (mul_inv_eq_one.mp hwone)

lemma pellToUnits_neg (z : Pell.Solution₁ 5) : pellToUnits (-z) = -pellToUnits z := by
  apply Units.ext
  change pellToR ((-z : Pell.Solution₁ 5) : ℤ√(5 : ℤ)) =
    -pellToR (z : ℤ√(5 : ℤ))
  exact map_neg pellToR (z : ℤ√(5 : ℤ))

/-- Every norm-one unit is a signed integral power of `α = φ²`. -/
theorem norm_one_unit_eq_signed_alpha_zpow (u : Rˣ)
    (hu : QuadraticAlgebra.norm (u : R) = 1) :
    ∃ m : ℤ, u = alphaUnit ^ m ∨ u = -(alphaUnit ^ m) := by
  let z : R := (u : R)
  have hz : QuadraticAlgebra.norm z = 1 := hu
  have hz3 : QuadraticAlgebra.norm (z ^ 3) = 1 := by
    rw [map_pow, hz]
    norm_num
  have heven : Even (z ^ 3).im := im_cube_even z hz
  let p : Pell.Solution₁ 5 := toPellOfEven (z ^ 3) hz3 heven
  have hpmap : pellToUnits p = u ^ 3 := by
    apply Units.ext
    rw [coe_pellToUnits]
    change pellToR (toPellOfEven (z ^ 3) hz3 heven : ℤ√(5 : ℤ)) =
      ((u ^ 3 : Rˣ) : R)
    rw [pellToR_toPellOfEven]
    rfl
  obtain ⟨m, hm | hm⟩ := pellFundamental_isFundamental.eq_zpow_or_neg_zpow p
  · have hmapped := congrArg pellToUnits hm
    rw [map_zpow, pellToUnits_fundamental] at hmapped
    rw [hpmap] at hmapped
    refine ⟨m, Or.inl (unit_cube_injective ?_)⟩
    change u ^ 3 = (alphaUnit ^ m) ^ 3
    calc
      u ^ 3 = (alphaUnit ^ (3 : ℤ)) ^ m := hmapped
      _ = alphaUnit ^ ((3 : ℤ) * m) := (zpow_mul alphaUnit 3 m).symm
      _ = alphaUnit ^ (m * (3 : ℤ)) := by rw [mul_comm]
      _ = (alphaUnit ^ m) ^ (3 : ℤ) := zpow_mul alphaUnit m 3
      _ = (alphaUnit ^ m) ^ (3 : ℕ) := zpow_natCast _ 3
  · have hmapped := congrArg pellToUnits hm
    rw [pellToUnits_neg, map_zpow, pellToUnits_fundamental] at hmapped
    rw [hpmap] at hmapped
    refine ⟨m, Or.inr (unit_cube_injective ?_)⟩
    change u ^ 3 = (-(alphaUnit ^ m)) ^ 3
    calc
      u ^ 3 = -((alphaUnit ^ (3 : ℤ)) ^ m) := hmapped
      _ = -(alphaUnit ^ ((3 : ℤ) * m)) := by rw [zpow_mul]
      _ = -(alphaUnit ^ (m * (3 : ℤ))) := by rw [mul_comm]
      _ = -((alphaUnit ^ m) ^ (3 : ℤ)) := by rw [zpow_mul]
      _ = -((alphaUnit ^ m) ^ (3 : ℕ)) := rfl
      _ = (-(alphaUnit ^ m)) ^ (3 : ℕ) := by
        rw [neg_pow]
        norm_num [pow_succ]

lemma norm_unit_eq_one_or_neg_one (u : Rˣ) :
    QuadraticAlgebra.norm (u : R) = 1 ∨ QuadraticAlgebra.norm (u : R) = -1 := by
  have hmul : QuadraticAlgebra.norm (u : R) *
      QuadraticAlgebra.norm ((u⁻¹ : Rˣ) : R) = 1 := by
    rw [← map_mul]
    norm_num
  exact (Int.mul_eq_one_iff_eq_one_or_neg_one.mp hmul).imp And.left And.left

/-- Every unit of the golden integer ring is a signed integral power of `φ`. -/
theorem unit_eq_signed_phi_zpow (u : Rˣ) :
    ∃ m : ℤ, u = phiUnit ^ m ∨ u = -(phiUnit ^ m) := by
  rcases norm_unit_eq_one_or_neg_one u with hu | hu
  · obtain ⟨m, hm | hm⟩ := norm_one_unit_eq_signed_alpha_zpow u hu
    · refine ⟨2 * m, Or.inl ?_⟩
      rw [hm, alphaUnit]
      exact (zpow_mul phiUnit 2 m).symm
    · refine ⟨2 * m, Or.inr ?_⟩
      rw [hm, alphaUnit]
      congr 1
      exact (zpow_mul phiUnit 2 m).symm
  · have huv : QuadraticAlgebra.norm ((u * phiUnit : Rˣ) : R) = 1 := by
      rw [Units.val_mul, map_mul, hu, coe_phiUnit, norm_phi]
      norm_num
    obtain ⟨m, hm | hm⟩ := norm_one_unit_eq_signed_alpha_zpow (u * phiUnit) huv
    · refine ⟨2 * m - 1, Or.inl ?_⟩
      have hu_eq : u = alphaUnit ^ m * phiUnit⁻¹ := by
        calc
          u = (u * phiUnit) * phiUnit⁻¹ := by simp
          _ = alphaUnit ^ m * phiUnit⁻¹ := by rw [hm]
      rw [hu_eq, alphaUnit]
      have htwo : (phiUnit ^ (2 : ℕ)) ^ m = phiUnit ^ ((2 : ℤ) * m) := by
        rw [← zpow_natCast]
        exact (zpow_mul phiUnit 2 m).symm
      calc
        (phiUnit ^ (2 : ℕ)) ^ m * phiUnit⁻¹ =
            phiUnit ^ ((2 : ℤ) * m) * phiUnit ^ (-1 : ℤ) := by
              rw [htwo, zpow_neg_one]
        _ = phiUnit ^ ((2 : ℤ) * m + (-1 : ℤ)) := (zpow_add phiUnit _ _).symm
        _ = phiUnit ^ (2 * m - 1) := by congr 1
    · refine ⟨2 * m - 1, Or.inr ?_⟩
      have hu_eq : u = -(alphaUnit ^ m) * phiUnit⁻¹ := by
        calc
          u = (u * phiUnit) * phiUnit⁻¹ := by simp
          _ = -(alphaUnit ^ m) * phiUnit⁻¹ := by rw [hm]
      rw [hu_eq, alphaUnit]
      have htwo : (phiUnit ^ (2 : ℕ)) ^ m = phiUnit ^ ((2 : ℤ) * m) := by
        rw [← zpow_natCast]
        exact (zpow_mul phiUnit 2 m).symm
      have hbase : (phiUnit ^ (2 : ℕ)) ^ m * phiUnit⁻¹ =
          phiUnit ^ (2 * m - 1) := by
        calc
          (phiUnit ^ (2 : ℕ)) ^ m * phiUnit⁻¹ =
              phiUnit ^ ((2 : ℤ) * m) * phiUnit ^ (-1 : ℤ) := by
                rw [htwo, zpow_neg_one]
          _ = phiUnit ^ ((2 : ℤ) * m + (-1 : ℤ)) := (zpow_add phiUnit _ _).symm
          _ = phiUnit ^ (2 * m - 1) := by congr 1
      calc
        -(phiUnit ^ (2 : ℕ)) ^ m * phiUnit⁻¹ =
            -((phiUnit ^ (2 : ℕ)) ^ m * phiUnit⁻¹) := neg_mul _ _
        _ = -phiUnit ^ (2 * m - 1) := congrArg Neg.neg hbase

end GoldenRing
end A374275Lean
