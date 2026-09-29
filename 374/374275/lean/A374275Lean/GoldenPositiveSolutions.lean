import A374275Lean.Definitions
import A374275Lean.GoldenAssociates

/-!
# Positive-wedge representations and associate classes

For positive `k`, reduced elements of norm `k` are exactly the solutions
`x > 0`, `y ≥ 0` of `x² + 3xy + y² = k`.  Consequently they are unique
representatives of the corresponding associate classes.
-/

namespace A374275Lean

open QuadraticAlgebra

namespace GoldenRing

/-- The ring element `x + αy`, in the basis `1, φ`. -/
def beta (x y : ℕ) : R := ⟨(x + y : ℕ), y⟩

lemma norm_beta (x y : ℕ) :
    QuadraticAlgebra.norm (beta x y) = (form x y : ℤ) := by
  rw [beta, form, QuadraticAlgebra.norm_def]
  push_cast
  ring

lemma beta_isReduced_iff (x y : ℕ) : IsReduced (beta x y) ↔ 0 < x := by
  rw [IsReduced, beta]
  constructor
  · intro h
    have hi := h.2
    change (y : ℤ) < (x + y : ℕ) at hi
    have hnat : y < x + y := by exact_mod_cast hi
    omega
  · intro hx
    constructor
    · change (0 : ℤ) ≤ (y : ℕ)
      exact_mod_cast (Nat.zero_le y)
    · change (y : ℤ) < (x + y : ℕ)
      have hnat : y < x + y := by omega
      exact_mod_cast hnat

lemma beta_injective : Function.Injective (Function.uncurry beta) := by
  rintro ⟨x, y⟩ ⟨x', y'⟩ h
  have hre := congrArg QuadraticAlgebra.re h
  have him := congrArg QuadraticAlgebra.im h
  change (y : ℤ) = y' at him
  have hy : y = y' := by exact_mod_cast him
  subst y'
  change ((x + y : ℕ) : ℤ) = (x' + y : ℕ) at hre
  have : x + y = x' + y := by exact_mod_cast hre
  exact Prod.ext (Nat.add_right_cancel this) rfl

/-- Positive-first-coordinate representations. -/
def positiveSolutions (k : ℕ) : Finset (ℕ × ℕ) :=
  ((Finset.range (k + 1)).product (Finset.range (k + 1))).filter
    fun pair ↦ 0 < pair.1 ∧ form pair.1 pair.2 = k

lemma mem_positiveSolutions_iff {k x y : ℕ} :
    (x, y) ∈ positiveSolutions k ↔ 0 < x ∧ form x y = k := by
  constructor
  · intro h
    exact (Finset.mem_filter.mp h).2
  · rintro ⟨hx, hform⟩
    apply Finset.mem_filter.mpr
    constructor
    · apply Finset.mem_product.mpr
      constructor <;> apply Finset.mem_range.mpr
      · have hxle : x ≤ form x y := by
          rw [form]
          nlinarith
        omega
      · have hyle : y ≤ form x y := by
          rw [form]
          nlinarith
        omega
    · exact ⟨hx, hform⟩

noncomputable def normClasses (k : ℕ) : Finset (Associates R) := by
  classical
  exact (positiveSolutions k).image fun p ↦ Associates.mk (beta p.1 p.2)

lemma mem_normClasses_iff_pair {k : ℕ} {a : Associates R} :
    a ∈ normClasses k ↔
      ∃ p ∈ positiveSolutions k, Associates.mk (beta p.1 p.2) = a := by
  classical
  simp [normClasses]

lemma classOfBeta_injective_on {k : ℕ} :
    Set.InjOn (fun p : ℕ × ℕ ↦ Associates.mk (beta p.1 p.2))
      (positiveSolutions k : Set (ℕ × ℕ)) := by
  intro p hp q hq heq
  have hpMem := (mem_positiveSolutions_iff.mp hp)
  have hqMem := (mem_positiveSolutions_iff.mp hq)
  have hkpos : 0 < k := by
    rw [← hpMem.2]
    rw [form]
    nlinarith
  have hbeta : beta p.1 p.2 = beta q.1 q.2 := by
    apply eq_of_associated_of_isReduced
    · exact (beta_isReduced_iff _ _).mpr hpMem.1
    · exact (beta_isReduced_iff _ _).mpr hqMem.1
    · rw [norm_beta, norm_beta, hpMem.2, hqMem.2]
    · rw [norm_beta, hpMem.2]
      exact_mod_cast hkpos
    · exact Associates.mk_eq_mk_iff_associated.mp heq
  exact beta_injective hbeta

lemma card_normClasses (k : ℕ) :
    (normClasses k).card = (positiveSolutions k).card := by
  classical
  unfold normClasses
  apply Finset.card_image_iff.mpr
  intro p hp q hq h
  exact classOfBeta_injective_on hp hq h

/-- Recover `(x,y)` from a reduced golden integer. -/
noncomputable def pairOfReduced (z : R) : ℕ × ℕ :=
  ((z.re - z.im).toNat, z.im.toNat)

lemma beta_pairOfReduced {z : R} (hz : IsReduced z) :
    beta (pairOfReduced z).1 (pairOfReduced z).2 = z := by
  have him : 0 ≤ z.im := hz.1
  have hx : 0 ≤ z.re - z.im := sub_nonneg.mpr hz.2.le
  apply QuadraticAlgebra.ext
  · simp [pairOfReduced, beta, Int.toNat_of_nonneg him, Int.toNat_of_nonneg hx]
  · simp [pairOfReduced, beta, Int.toNat_of_nonneg him]

lemma pairOfReduced_pos {z : R} (hz : IsReduced z) :
    0 < (pairOfReduced z).1 := by
  rw [pairOfReduced]
  apply Nat.pos_of_ne_zero
  intro hzero
  have hle : z.re - z.im ≤ 0 := Int.toNat_eq_zero.mp hzero
  exact (not_le_of_gt (sub_pos.mpr hz.2)) hle

/-- For positive `k`, `normClasses k` consists exactly of the associate
classes having a representative of norm `k`. -/
theorem mem_normClasses_iff {k : ℕ} (hk : 0 < k) {a : Associates R} :
    a ∈ normClasses k ↔
      ∃ z : R, Associates.mk z = a ∧ QuadraticAlgebra.norm z = (k : ℤ) := by
  constructor
  · intro ha
    obtain ⟨p, hp, rfl⟩ := mem_normClasses_iff_pair.mp ha
    refine ⟨beta p.1 p.2, rfl, ?_⟩
    rw [norm_beta, (mem_positiveSolutions_iff.mp hp).2]
  · rintro ⟨z, rfl, hzNorm⟩
    have hzPos : 0 < QuadraticAlgebra.norm z := by
      rw [hzNorm]
      exact_mod_cast hk
    obtain ⟨w, hwAssoc, hwNorm, hwRed⟩ := exists_reduced_associate hzPos
    apply mem_normClasses_iff_pair.mpr
    refine ⟨pairOfReduced w, ?_, ?_⟩
    · apply mem_positiveSolutions_iff.mpr
      constructor
      · exact pairOfReduced_pos hwRed
      · have hnormPair := norm_beta (pairOfReduced w).1 (pairOfReduced w).2
        rw [beta_pairOfReduced hwRed, hwNorm, hzNorm] at hnormPair
        exact_mod_cast hnormPair.symm
    · rw [beta_pairOfReduced hwRed]
      exact Associates.mk_eq_mk_iff_associated.mpr hwAssoc

end GoldenRing
end A374275Lean
