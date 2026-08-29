import A330432Lean.ProductFormula

/-!
# Lists encoded by nested choices

This file connects the recursive type used by the product argument with
ordinary duplicate-free lists satisfying the prescribed membership
conditions.
-/

namespace A330432Lean

open Finset

namespace Choices

/-- Forget the dependent encoding and retain the chosen values in order. -/
def toList {α : Type u} [DecidableEq α] {used : Finset α} :
    {sets : List (Finset α)} → Choices used sets → List α
  | [], _ => []
  | _s :: _ss, c => c.1.1 :: toList c.2

@[simp] theorem length_toList {α : Type u} [DecidableEq α]
    {used : Finset α} {sets : List (Finset α)}
    (c : Choices used sets) : c.toList.length = sets.length := by
  induction sets generalizing used with
  | nil => simp [toList]
  | cons s ss ih =>
      rcases c with ⟨x, c⟩
      simp [toList, ih]

theorem not_mem_used_of_mem_toList {α : Type u} [DecidableEq α]
    {used : Finset α} {sets : List (Finset α)}
    (c : Choices used sets) {y : α} (hy : y ∈ c.toList) :
    y ∉ used := by
  induction sets generalizing used with
  | nil => simp [toList] at hy
  | cons s ss ih =>
      rcases c with ⟨x, c⟩
      simp only [toList, List.mem_cons] at hy
      rcases hy with rfl | hy
      · exact (Finset.mem_sdiff.mp x.2).2
      · have hnot := ih c hy
        exact fun hmem => hnot (Finset.mem_insert_of_mem hmem)

theorem nodup_toList {α : Type u} [DecidableEq α]
    {used : Finset α} {sets : List (Finset α)}
    (c : Choices used sets) : c.toList.Nodup := by
  induction sets generalizing used with
  | nil => simp [toList]
  | cons s ss ih =>
      rcases c with ⟨x, c⟩
      simp only [toList, List.nodup_cons]
      constructor
      · intro hx
        exact c.not_mem_used_of_mem_toList hx (Finset.mem_insert_self x.1 used)
      · exact ih c

theorem forall₂_toList {α : Type u} [DecidableEq α]
    {used : Finset α} {sets : List (Finset α)}
    (c : Choices used sets) :
    List.Forall₂ (fun x s => x ∈ s) c.toList sets := by
  induction sets generalizing used with
  | nil => simp [toList]
  | cons s ss ih =>
      rcases c with ⟨x, c⟩
      exact List.Forall₂.cons (Finset.mem_sdiff.mp x.2).1
        (ih c)

theorem toList_injective {α : Type u} [DecidableEq α]
    (used : Finset α) (sets : List (Finset α)) :
    Function.Injective (@toList α _ used sets) := by
  induction sets generalizing used with
  | nil =>
      intro a b _
      change PUnit at a b
      cases a
      cases b
      rfl
  | cons s ss ih =>
      rintro ⟨x, a⟩ ⟨y, b⟩ h
      simp only [toList, List.cons.injEq] at h
      have hxy : x = y := Subtype.ext h.1
      subst y
      have hab : a = b := ih (used := insert x.1 used) h.2
      subst b
      rfl

end Choices

/-- The intrinsic properties of a list represented by `Choices used sets`. -/
def IsChoiceList {α : Type u} [DecidableEq α]
    (used : Finset α) (sets : List (Finset α)) (xs : List α) : Prop :=
  xs.length = sets.length ∧
  xs.Nodup ∧
  (∀ x ∈ xs, x ∉ used) ∧
  List.Forall₂ (fun x s => x ∈ s) xs sets

theorem Choices.isChoiceList {α : Type u} [DecidableEq α]
    {used : Finset α} {sets : List (Finset α)}
    (c : Choices used sets) : IsChoiceList used sets c.toList := by
  exact ⟨c.length_toList, c.nodup_toList,
    fun _x _hx => c.not_mem_used_of_mem_toList _hx, c.forall₂_toList⟩

/-- Every list with the intrinsic properties has a recursive encoding. -/
theorem exists_choices_of_isChoiceList {α : Type u} [DecidableEq α]
    {used : Finset α} {sets : List (Finset α)} {xs : List α}
    (h : IsChoiceList used sets xs) :
    ∃ c : Choices used sets, c.toList = xs := by
  induction sets generalizing used xs with
  | nil =>
      have hnil : xs = [] := List.eq_nil_of_length_eq_zero (by simpa using h.1)
      subst xs
      exact ⟨PUnit.unit, rfl⟩
  | cons s ss ih =>
      cases xs with
      | nil => simp [IsChoiceList] at h
      | cons x xs =>
          have hlength : xs.length = ss.length := by
            simpa using h.1
          have hnodup := List.nodup_cons.mp h.2.1
          have havoid := h.2.2.1
          have hrel := h.2.2.2
          cases hrel with
          | cons hx htailrel =>
              have hxnot : x ∉ used := havoid x (by simp)
              let x' : {y // y ∈ s \ used} :=
                ⟨x, Finset.mem_sdiff.mpr ⟨hx, hxnot⟩⟩
              have htailavoid : ∀ y ∈ xs, y ∉ insert x used := by
                intro y hy hymem
                rw [Finset.mem_insert] at hymem
                rcases hymem with rfl | hyused
                · exact hnodup.1 hy
                · exact havoid y (by simp [hy]) hyused
              have htail : IsChoiceList (insert x used) ss xs :=
                ⟨hlength, hnodup.2, htailavoid, htailrel⟩
              obtain ⟨c, hc⟩ := ih htail
              refine ⟨⟨x', c⟩, ?_⟩
              simp [Choices.toList, x', hc]

/-- Ordinary lists satisfying the exact conditions encoded by `Choices`. -/
def ChoiceList {α : Type u} [DecidableEq α]
    (used : Finset α) (sets : List (Finset α)) :=
  {xs : List α // IsChoiceList used sets xs}

/-- The recursive encoding and its ordinary-list description are equivalent. -/
noncomputable def choicesEquivChoiceList {α : Type u} [DecidableEq α]
    (used : Finset α) (sets : List (Finset α)) :
    Choices used sets ≃ ChoiceList used sets :=
  Equiv.ofBijective
    (fun c => ⟨c.toList, c.isChoiceList⟩)
    ⟨by
      intro a b h
      apply Choices.toList_injective used sets
      exact congrArg Subtype.val h,
    by
      rintro ⟨xs, hxs⟩
      obtain ⟨c, hc⟩ := exists_choices_of_isChoiceList hxs
      exact ⟨c, Subtype.ext hc⟩⟩

end A330432Lean
