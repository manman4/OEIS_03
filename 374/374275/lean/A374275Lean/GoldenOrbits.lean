import A374275Lean.GoldenPositiveSolutions

/-!
# A374275 representations as conjugation orbits

For `k > 0`, A374275 solutions `0 ≤ x ≤ y` are in bijection with the
conjugation orbits of associate classes of norm `k`.
-/

namespace A374275Lean

open QuadraticAlgebra

namespace GoldenRing

abbrev NormClass (k : ℕ) := {a : Associates R // a ∈ normClasses k}

lemma conjAssoc_mem_normClasses {k : ℕ} (hk : 0 < k) {a : Associates R}
    (ha : a ∈ normClasses k) : conjAssoc a ∈ normClasses k := by
  obtain ⟨z, rfl, hz⟩ := (mem_normClasses_iff hk).mp ha
  apply (mem_normClasses_iff hk).mpr
  refine ⟨star z, rfl, ?_⟩
  rw [QuadraticAlgebra.norm_star, hz]

def conjNormClass {k : ℕ} (hk : 0 < k) : NormClass k → NormClass k :=
  fun a ↦ ⟨conjAssoc a.1, conjAssoc_mem_normClasses hk a.2⟩

@[simp] lemma conjNormClass_val {k : ℕ} (hk : 0 < k) (a : NormClass k) :
    (conjNormClass hk a).1 = conjAssoc a.1 := rfl

@[simp] lemma conjNormClass_involutive {k : ℕ} (hk : 0 < k) (a : NormClass k) :
    conjNormClass hk (conjNormClass hk a) = a := by
  apply Subtype.ext
  exact conjAssoc_involutive a.1

def conjSetoid {k : ℕ} (hk : 0 < k) : Setoid (NormClass k) where
  r a b := b = a ∨ b = conjNormClass hk a
  iseqv := by
    constructor
    · intro a
      exact Or.inl rfl
    · intro a b hab
      rcases hab with rfl | rfl
      · exact Or.inl rfl
      · exact Or.inr (conjNormClass_involutive hk a).symm
    · intro a b c hab hbc
      rcases hab with rfl | rfl <;> rcases hbc with rfl | rfl
      · exact Or.inl rfl
      · exact Or.inr rfl
      · exact Or.inr rfl
      · exact Or.inl (conjNormClass_involutive hk a)

abbrev NormClassOrbit {k : ℕ} (hk : 0 < k) := Quotient (conjSetoid hk)

lemma alpha_mul_star_beta (x y : ℕ) : alpha * star (beta x y) = beta y x := by
  apply QuadraticAlgebra.ext
  all_goals simp [alpha, beta]
  ring

lemma conjAssoc_mk_beta (x y : ℕ) :
    conjAssoc (Associates.mk (beta x y)) = Associates.mk (beta y x) := by
  rw [conjAssoc_mk]
  apply Associates.mk_eq_mk_iff_associated.mpr
  have hunit : IsUnit alpha := by
    rw [← coe_alphaUnit]
    exact Units.isUnit alphaUnit
  exact (associated_unit_mul_left (star (beta x y)) alpha hunit).symm.trans
    (Associated.of_eq (alpha_mul_star_beta x y))

lemma second_pos_of_mem_solutions_of_pos {k x y : ℕ} (hk : 0 < k)
    (hxy : (x, y) ∈ solutions k) : 0 < y := by
  have h := mem_solutions_iff.mp hxy
  by_contra hy
  have hy0 : y = 0 := by omega
  have hx0 : x = 0 := by omega
  subst x
  subst y
  simp [form] at h
  omega

lemma swapped_mem_positiveSolutions {k x y : ℕ} (hk : 0 < k)
    (hxy : (x, y) ∈ solutions k) : (y, x) ∈ positiveSolutions k := by
  have h := mem_solutions_iff.mp hxy
  apply mem_positiveSolutions_iff.mpr
  constructor
  · exact second_pos_of_mem_solutions_of_pos hk hxy
  · calc
      form y x = form x y := by simp [form]; ring
      _ = k := h.2

abbrev SolutionType (k : ℕ) := {p : ℕ × ℕ // p ∈ solutions k}

noncomputable def solutionClass {k : ℕ} (hk : 0 < k) (s : SolutionType k) :
    NormClass k :=
  ⟨Associates.mk (beta s.1.2 s.1.1), by
    apply mem_normClasses_iff_pair.mpr
    exact ⟨(s.1.2, s.1.1), swapped_mem_positiveSolutions hk s.2, rfl⟩⟩

noncomputable def solutionToOrbit {k : ℕ} (hk : 0 < k) :
    SolutionType k → NormClassOrbit hk := fun s ↦
  Quotient.mk (conjSetoid hk) (solutionClass hk s)

lemma beta_eq_of_class_eq_of_pos {k u v u' v' : ℕ}
    (huv : (u, v) ∈ positiveSolutions k)
    (huv' : (u', v') ∈ positiveSolutions k)
    (hclass : Associates.mk (beta u v) = Associates.mk (beta u' v')) :
    (u, v) = (u', v') := by
  apply classOfBeta_injective_on huv huv' hclass

theorem solutionToOrbit_injective {k : ℕ} (hk : 0 < k) :
    Function.Injective (solutionToOrbit hk) := by
  intro s t hst
  change Quotient.mk (conjSetoid hk) (solutionClass hk s) =
    Quotient.mk (conjSetoid hk) (solutionClass hk t) at hst
  have hrel := Quotient.exact hst
  change solutionClass hk t = solutionClass hk s ∨
    solutionClass hk t = conjNormClass hk (solutionClass hk s) at hrel
  rcases hrel with hsame | hconj
  · have hc : Associates.mk (beta s.1.2 s.1.1) =
        Associates.mk (beta t.1.2 t.1.1) := by
      simpa [solutionClass] using congrArg Subtype.val hsame.symm
    have hpairs := beta_eq_of_class_eq_of_pos
      (swapped_mem_positiveSolutions hk s.2)
      (swapped_mem_positiveSolutions hk t.2) hc
    have hfst := congrArg Prod.fst hpairs
    have hsnd := congrArg Prod.snd hpairs
    apply Subtype.ext
    exact Prod.ext hsnd hfst
  · have hc : Associates.mk (beta t.1.2 t.1.1) =
        Associates.mk (beta s.1.1 s.1.2) := by
      have := congrArg Subtype.val hconj
      change Associates.mk (beta t.1.2 t.1.1) =
        conjAssoc (Associates.mk (beta s.1.2 s.1.1)) at this
      rw [conjAssoc_mk_beta] at this
      exact this
    by_cases hsx : s.1.1 = 0
    · have hsClass : Associates.mk (beta s.1.1 s.1.2) =
          Associates.mk (beta s.1.2 s.1.1) := by
        calc
          Associates.mk (beta s.1.1 s.1.2) = Associates.mk (beta 0 s.1.2) := by rw [hsx]
          _ = conjAssoc (Associates.mk (beta s.1.2 0)) :=
            (conjAssoc_mk_beta s.1.2 0).symm
          _ = Associates.mk (beta s.1.2 0) := by simp [beta]
          _ = Associates.mk (beta s.1.2 s.1.1) := by rw [hsx]
      have hc' : Associates.mk (beta t.1.2 t.1.1) =
          Associates.mk (beta s.1.2 s.1.1) := hc.trans hsClass
      have hpairs := beta_eq_of_class_eq_of_pos
        (swapped_mem_positiveSolutions hk t.2)
        (swapped_mem_positiveSolutions hk s.2) hc'
      have hfst := congrArg Prod.fst hpairs.symm
      have hsnd := congrArg Prod.snd hpairs.symm
      apply Subtype.ext
      exact Prod.ext hsnd hfst
    · have hsxPos : 0 < s.1.1 := Nat.pos_of_ne_zero hsx
      have hsPositive : (s.1.1, s.1.2) ∈ positiveSolutions k := by
        apply mem_positiveSolutions_iff.mpr
        exact ⟨hsxPos, (mem_solutions_iff.mp s.2).2⟩
      have hpairs := beta_eq_of_class_eq_of_pos
        (swapped_mem_positiveSolutions hk t.2) hsPositive hc
      have hfst := congrArg Prod.fst hpairs
      have hsnd := congrArg Prod.snd hpairs
      have hsOrd := (mem_solutions_iff.mp s.2).1
      have htOrd := (mem_solutions_iff.mp t.2).1
      have hsRev : s.1.2 ≤ s.1.1 := by
        rw [← hsnd, ← hfst]
        exact htOrd
      have heq : s.1.1 = s.1.2 := Nat.le_antisymm hsOrd hsRev
      have hstPair : s.1 = t.1 := by
        apply Prod.ext
        · exact heq.trans hsnd.symm
        · exact heq.symm.trans hfst.symm
      exact Subtype.ext hstPair

theorem solutionToOrbit_surjective {k : ℕ} (hk : 0 < k) :
    Function.Surjective (solutionToOrbit hk) := by
  intro q
  let c : NormClass k := Quotient.out q
  have hcq : (Quotient.mk (conjSetoid hk) c : NormClassOrbit hk) = q := Quotient.out_eq q
  obtain ⟨p, hp, hpclass⟩ := mem_normClasses_iff_pair.mp c.2
  rcases p with ⟨u, v⟩
  by_cases hle : v ≤ u
  · have hs : (v, u) ∈ solutions k := by
      apply mem_solutions_iff.mpr
      refine ⟨hle, ?_⟩
      calc
        form v u = form u v := by simp [form]; ring
        _ = k := (mem_positiveSolutions_iff.mp hp).2
    refine ⟨⟨(v, u), hs⟩, ?_⟩
    rw [← hcq]
    apply Quotient.sound
    left
    apply Subtype.ext
    simpa [solutionClass] using hpclass.symm
  · have huv : u < v := lt_of_not_ge hle
    have hs : (u, v) ∈ solutions k := by
      apply mem_solutions_iff.mpr
      exact ⟨huv.le, (mem_positiveSolutions_iff.mp hp).2⟩
    refine ⟨⟨(u, v), hs⟩, ?_⟩
    rw [← hcq]
    apply Quotient.sound
    right
    apply Subtype.ext
    change c.1 = conjAssoc (Associates.mk (beta v u))
    rw [← hpclass, conjAssoc_mk_beta]

noncomputable def solutionsEquivOrbits {k : ℕ} (hk : 0 < k) :
    SolutionType k ≃ NormClassOrbit hk :=
  Equiv.ofBijective (solutionToOrbit hk)
    ⟨solutionToOrbit_injective hk, solutionToOrbit_surjective hk⟩

theorem representationCount_eq_natCard_orbits {k : ℕ} (hk : 0 < k) :
    representationCount k = Nat.card (NormClassOrbit hk) := by
  rw [representationCount]
  calc
    (solutions k).card = Fintype.card (SolutionType k) := (Fintype.card_coe _).symm
    _ = Nat.card (SolutionType k) := by rw [Nat.card_eq_fintype_card]
    _ = Nat.card (NormClassOrbit hk) := Nat.card_congr (solutionsEquivOrbits hk)

end GoldenRing
end A374275Lean
