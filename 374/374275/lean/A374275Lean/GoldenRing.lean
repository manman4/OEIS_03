/-
Copyright (c) 2026 Seiichi Manyama. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Seiichi Manyama

The Euclidean-domain construction follows the smart-rounding pattern used by
Barinder S. Banwait in the Ramanujan--Nagell formalization (Apache-2.0),
adapted here to the real quadratic ring of discriminant 5.
-/

import Mathlib.Algebra.QuadraticAlgebra.Basic
import Mathlib.Algebra.Order.Round
import Mathlib.Data.Rat.Floor
import Mathlib.RingTheory.PrincipalIdealDomain
import Mathlib.RingTheory.UniqueFactorizationDomain.Defs
import Mathlib.Tactic

/-!
# The golden integer ring

`GoldenRing` is `ℤ[φ]`, where `φ² = φ + 1`.  Its norm is
`N(a + bφ) = a² + ab - b²`.  The absolute norm is Euclidean.
-/

namespace A374275Lean
namespace GoldenRing

open QuadraticAlgebra

/-- The ring `ℤ[(1 + √5)/2]`. -/
abbrev R : Type := QuadraticAlgebra ℤ 1 1

/-- The golden ratio generator, satisfying `φ² = φ + 1`. -/
def phi : R := ⟨0, 1⟩

/-- The totally positive norm-one unit `α = φ² = 1 + φ`. -/
def alpha : R := ⟨1, 1⟩

lemma phi_sq : phi ^ 2 = phi + 1 := rfl

lemma alpha_eq_phi_sq : alpha = phi ^ 2 := by
  rw [phi_sq]
  rfl

lemma norm_eq (x y : ℤ) :
    QuadraticAlgebra.norm (⟨x, y⟩ : R) = x ^ 2 + x * y - y ^ 2 := by
  rw [QuadraticAlgebra.norm_def]
  ring

lemma four_norm_eq (z : R) :
    4 * QuadraticAlgebra.norm z = (2 * z.re + z.im) ^ 2 - 5 * z.im ^ 2 := by
  rw [QuadraticAlgebra.norm_def]
  ring

private lemma five_not_square_rat : ¬ IsSquare (5 : ℚ) := by
  norm_num

lemma norm_eq_zero_iff (z : R) : QuadraticAlgebra.norm z = 0 ↔ z = 0 := by
  constructor
  · intro hz
    by_cases hy : z.im = 0
    · have hx : z.re = 0 := by
        rw [QuadraticAlgebra.norm_def, hy] at hz
        nlinarith
      exact QuadraticAlgebra.ext hx hy
    · exfalso
      apply five_not_square_rat
      refine ⟨2 * (z.re : ℚ) / z.im + 1, ?_⟩
      have hyq : (z.im : ℚ) ≠ 0 := by exact_mod_cast hy
      rw [QuadraticAlgebra.norm_def] at hz
      have hzInt : z.re * z.re + z.re * z.im - z.im * z.im = 0 := by
        simpa using hz
      have hzq : (z.re : ℚ) * z.re + (z.re : ℚ) * z.im - (z.im : ℚ) * z.im = 0 := by
        exact_mod_cast hzInt
      field_simp
      nlinarith [hzq]
  · rintro rfl
    exact QuadraticAlgebra.norm_zero

lemma norm_ne_zero {z : R} (hz : z ≠ 0) : QuadraticAlgebra.norm z ≠ 0 :=
  fun h ↦ hz ((norm_eq_zero_iff z).mp h)

private lemma b_mul_star_eq_norm (b : R) :
    b * star b = ((QuadraticAlgebra.norm b : ℤ) : R) := by
  apply QuadraticAlgebra.ext
  · simp only [QuadraticAlgebra.re_mul, QuadraticAlgebra.re_star,
      QuadraticAlgebra.im_star, QuadraticAlgebra.re_intCast,
      QuadraticAlgebra.norm_def, Int.cast_id]
    ring
  · simp only [QuadraticAlgebra.im_mul, QuadraticAlgebra.re_star,
      QuadraticAlgebra.im_star, QuadraticAlgebra.im_intCast]
    ring

private lemma norm_mul_norm_rem_eq (a b q : R) (hb : b ≠ 0) :
    QuadraticAlgebra.norm b * QuadraticAlgebra.norm (a - b * q) =
      QuadraticAlgebra.norm
        (a * star b - ((QuadraticAlgebra.norm b : ℤ) : R) * q) := by
  have hN : QuadraticAlgebra.norm b ≠ 0 := norm_ne_zero hb
  have hRing :
      ((QuadraticAlgebra.norm b : ℤ) : R) * (a - b * q) =
        b * (a * star b - ((QuadraticAlgebra.norm b : ℤ) : R) * q) := by
    have hbs := b_mul_star_eq_norm b
    calc
      ((QuadraticAlgebra.norm b : ℤ) : R) * (a - b * q)
          = (b * star b) * (a - b * q) := by rw [← hbs]
      _ = b * (a * star b - (b * star b) * q) := by ring
      _ = b * (a * star b - ((QuadraticAlgebra.norm b : ℤ) : R) * q) := by
        rw [hbs]
  have hNorm := congrArg QuadraticAlgebra.norm hRing
  rw [map_mul, map_mul, QuadraticAlgebra.norm_intCast] at hNorm
  have hSq : QuadraticAlgebra.norm b ^ 2 =
      QuadraticAlgebra.norm b * QuadraticAlgebra.norm b := sq _
  have hCancel :
      QuadraticAlgebra.norm b *
          (QuadraticAlgebra.norm b * QuadraticAlgebra.norm (a - b * q)) =
        QuadraticAlgebra.norm b *
          QuadraticAlgebra.norm
            (a * star b - ((QuadraticAlgebra.norm b : ℤ) : R) * q) := by
    rw [← mul_assoc, ← hSq]
    exact hNorm
  exact mul_left_cancel₀ hN hCancel

/-- Coordinatewise nearest-integer quotient in the norm-Euclidean algorithm. -/
noncomputable def quot (a b : R) : R :=
  let N : ℤ := QuadraticAlgebra.norm b
  if N = 0 then 0
  else
    let s : R := a * star b
    let m : ℤ := round ((s.re : ℚ) / N)
    let n : ℤ := round ((s.im : ℚ) / N)
    ⟨m, n⟩

noncomputable def rem (a b : R) : R := a - b * quot a b

@[simp] lemma quot_zero (a : R) : quot a 0 = 0 := by
  unfold quot
  simp

lemma quot_mul_add_rem_eq (a b : R) : b * quot a b + rem a b = a := by
  unfold rem
  ring

private lemma four_abs_norm_residual_lt (a b : R) (hb : b ≠ 0) :
    4 * |QuadraticAlgebra.norm (rem a b)| <
      4 * |QuadraticAlgebra.norm b| := by
  set N : ℤ := QuadraticAlgebra.norm b with hN_def
  have hN_ne : N ≠ 0 := norm_ne_zero hb
  have hN_abs_pos : 0 < |N| := abs_pos.mpr hN_ne
  set s : R := a * star b with hs_def
  set m : ℤ := round ((s.re : ℚ) / N) with hm_def
  set n : ℤ := round ((s.im : ℚ) / N) with hn_def
  have hquot : quot a b = (⟨m, n⟩ : R) := by
    change (if QuadraticAlgebra.norm b = 0 then (0 : R) else _) = _
    rw [if_neg hN_ne]
  set u : ℤ := s.re - N * m with hu_def
  set v : ℤ := s.im - N * n with hv_def
  have hNq : (N : ℚ) ≠ 0 := by exact_mod_cast hN_ne
  have huAbs : |2 * u| ≤ |N| := by
    have hround : |(s.re : ℚ) / N - m| ≤ 1 / 2 := abs_sub_round _
    have heq : ((s.re : ℚ) / N - m) * (2 * N) = ((2 * u : ℤ) : ℚ) := by
      push_cast [hu_def]
      field_simp
    have hmul := mul_le_mul_of_nonneg_right hround (abs_nonneg ((2 * N : ℤ) : ℚ))
    have heq' : ((s.re : ℚ) / N - m) * (((2 * N : ℤ) : ℚ)) =
        ((2 * u : ℤ) : ℚ) := by
      simpa only [Int.cast_mul, Int.cast_ofNat] using heq
    rw [← abs_mul, heq'] at hmul
    have hmul' : (((2 * |u| : ℤ) : ℚ)) ≤ ((|N| : ℤ) : ℚ) := by
      calc
        (((2 * |u| : ℤ) : ℚ)) ≤ (1 / 2 : ℚ) * (((2 * |N| : ℤ) : ℚ)) := by
          simpa [abs_mul] using hmul
        _ = ((|N| : ℤ) : ℚ) := by push_cast; ring
    have hplain : 2 * |u| ≤ |N| := by exact_mod_cast hmul'
    simpa [abs_mul] using hplain
  have hvAbs : |2 * v| ≤ |N| := by
    have hround : |(s.im : ℚ) / N - n| ≤ 1 / 2 := abs_sub_round _
    have heq : ((s.im : ℚ) / N - n) * (2 * N) = ((2 * v : ℤ) : ℚ) := by
      push_cast [hv_def]
      field_simp
    have hmul := mul_le_mul_of_nonneg_right hround (abs_nonneg ((2 * N : ℤ) : ℚ))
    have heq' : ((s.im : ℚ) / N - n) * (((2 * N : ℤ) : ℚ)) =
        ((2 * v : ℤ) : ℚ) := by
      simpa only [Int.cast_mul, Int.cast_ofNat] using heq
    rw [← abs_mul, heq'] at hmul
    have hmul' : (((2 * |v| : ℤ) : ℚ)) ≤ ((|N| : ℤ) : ℚ) := by
      calc
        (((2 * |v| : ℤ) : ℚ)) ≤ (1 / 2 : ℚ) * (((2 * |N| : ℤ) : ℚ)) := by
          simpa [abs_mul] using hmul
        _ = ((|N| : ℤ) : ℚ) := by push_cast; ring
    have hplain : 2 * |v| ≤ |N| := by exact_mod_cast hmul'
    simpa [abs_mul] using hplain
  have huSq : 4 * u ^ 2 ≤ N ^ 2 := by
    have hs : (2 * u) ^ 2 ≤ N ^ 2 := (sq_le_sq).2 huAbs
    nlinarith
  have hvSq : 4 * v ^ 2 ≤ N ^ 2 := by
    have hs : (2 * v) ^ 2 ≤ N ^ 2 := (sq_le_sq).2 hvAbs
    nlinarith
  have huv : 2 * |u * v| ≤ u ^ 2 + v ^ 2 := by
    calc
      2 * |u * v| = 2 * |u| * |v| := by rw [abs_mul]; ring
      _ ≤ |u| ^ 2 + |v| ^ 2 := by nlinarith [sq_nonneg (|u| - |v|)]
      _ = u ^ 2 + v ^ 2 := by rw [sq_abs, sq_abs]
  have htriangle : |u ^ 2 + u * v - v ^ 2| ≤ u ^ 2 + |u * v| + v ^ 2 := by
    calc
      |u ^ 2 + u * v - v ^ 2| ≤ |u ^ 2| + |u * v| + |v ^ 2| := by
        calc
          |u ^ 2 + u * v - v ^ 2| = |(u ^ 2 + u * v) + -(v ^ 2)| := by ring
          _ ≤ |u ^ 2 + u * v| + |-(v ^ 2)| := abs_add_le _ _
          _ = |u ^ 2 + u * v| + |v ^ 2| := by rw [abs_neg]
          _ ≤ |u ^ 2| + |u * v| + |v ^ 2| := by
            gcongr
            exact abs_add_le _ _
      _ = u ^ 2 + |u * v| + v ^ 2 := by simp only [abs_sq]
  have hresidual :
      N * QuadraticAlgebra.norm (rem a b) = u ^ 2 + u * v - v ^ 2 := by
    unfold rem
    rw [hquot]
    have h := norm_mul_norm_rem_eq a b (⟨m, n⟩ : R) hb
    rw [← hN_def] at h
    rw [h]
    have hre :
        (a * star b - ((N : ℤ) : R) * (⟨m, n⟩ : R)).re = u := by
      change s.re - (((N : ℤ) : R) * (⟨m, n⟩ : R)).re = u
      push_cast [hu_def]
      change s.re - (N * m + 1 * 0 * n) = s.re - N * m
      ring
    have him :
        (a * star b - ((N : ℤ) : R) * (⟨m, n⟩ : R)).im = v := by
      change s.im - (((N : ℤ) : R) * (⟨m, n⟩ : R)).im = v
      push_cast [hv_def]
      change s.im - (N * n + 0 * m + 1 * 0 * n) = s.im - N * n
      ring
    rw [QuadraticAlgebra.norm_def, hre, him]
    ring
  have hNormBound : 4 * |u ^ 2 + u * v - v ^ 2| ≤ 3 * N ^ 2 := by
    have hsum : 2 * (u ^ 2 + |u * v| + v ^ 2) ≤ 3 * (u ^ 2 + v ^ 2) := by
      nlinarith
    nlinarith
  have habsResidual :
      |N| * |QuadraticAlgebra.norm (rem a b)| =
        |u ^ 2 + u * v - v ^ 2| := by
    rw [← abs_mul, hresidual]
  have hremLt : |QuadraticAlgebra.norm (rem a b)| < |N| := by
    by_contra hnot
    have hle : |N| ≤ |QuadraticAlgebra.norm (rem a b)| := le_of_not_gt hnot
    have hmul : |N| * |N| ≤ |N| * |QuadraticAlgebra.norm (rem a b)| :=
      mul_le_mul_of_nonneg_left hle (abs_nonneg N)
    rw [habsResidual] at hmul
    nlinarith [sq_abs N]
  have hlt : 4 * |QuadraticAlgebra.norm (rem a b)| < 4 * |N| := by omega
  simpa [hN_def] using hlt

private noncomputable def normMeasure (a : R) : ℕ :=
  Int.natAbs (QuadraticAlgebra.norm a)

private lemma natAbs_norm_rem_lt (a : R) {b : R} (hb : b ≠ 0) :
    normMeasure (rem a b) < normMeasure b := by
  unfold normMeasure
  have h := four_abs_norm_residual_lt a b hb
  zify
  simpa using h

private lemma norm_mul_left_not_lt (a : R) {b : R} (hb : b ≠ 0) :
    ¬ normMeasure (a * b) < normMeasure a := by
  unfold normMeasure
  have hbAbs : 1 ≤ |QuadraticAlgebra.norm b| := by
    exact (Int.one_le_abs (norm_ne_zero hb))
  intro h
  zify at h
  rw [map_mul, abs_mul] at h
  have haAbs : 0 ≤ |QuadraticAlgebra.norm a| := abs_nonneg _
  nlinarith

/-- `ℤ[(1+√5)/2]` is Euclidean for the absolute field norm. -/
noncomputable instance instEuclideanDomain : EuclideanDomain R where
  quotient := quot
  quotient_zero := quot_zero
  remainder := rem
  quotient_mul_add_remainder_eq := quot_mul_add_rem_eq
  r := fun a b ↦ normMeasure a < normMeasure b
  r_wellFounded := (measure normMeasure).wf
  remainder_lt := natAbs_norm_rem_lt
  mul_left_not_lt := norm_mul_left_not_lt

instance instPrincipalIdealRing : IsPrincipalIdealRing R :=
  EuclideanDomain.to_principal_ideal_domain

instance instUniqueFactorizationMonoid : UniqueFactorizationMonoid R := inferInstance

end GoldenRing
end A374275Lean
