import Mathlib.Data.Fintype.BigOperators
import Mathlib.Data.List.Pairwise

/-!
# Injective choices from nested finite sets

This file proves the general finite counting lemma used for A330432.
-/

namespace A330432Lean

open Finset

/-- Successively choose one unused element from each finite set. -/
def Choices {α : Type u} [DecidableEq α] (used : Finset α) :
    List (Finset α) → Type u
  | [] => PUnit
  | s :: ss => Σ x : {x // x ∈ s \ used}, Choices (insert x.1 used) ss

noncomputable def Choices.fintype {α : Type*} [DecidableEq α]
    (used : Finset α) (sets : List (Finset α)) : Fintype (Choices used sets) := by
  induction sets generalizing used with
  | nil =>
      simp only [Choices]
      infer_instance
  | cons s ss ih =>
      simp only [Choices]
      letI (x : {x // x ∈ s \ used}) :
          Fintype (Choices (insert x.1 used) ss) :=
        ih (insert x.1 used)
      infer_instance

noncomputable instance {α : Type*} [DecidableEq α]
    (used : Finset α) (sets : List (Finset α)) :
    Fintype (Choices used sets) :=
  Choices.fintype used sets

/-- Product of the numbers of choices remaining at successive stages. -/
def choiceProduct {α : Type*} (usedCard : ℕ) :
    List (Finset α) → ℕ
  | [] => 1
  | s :: ss => (s.card - usedCard) * choiceProduct (usedCard + 1) ss

/-- Counting injective choices from nested finite sets. -/
theorem card_choices_eq_choiceProduct {α : Type*} [DecidableEq α]
    (used : Finset α) (sets : List (Finset α))
    (hnested : sets.Pairwise (· ⊆ ·))
    (hused : ∀ s ∈ sets, used ⊆ s) :
    Fintype.card (Choices used sets) = choiceProduct used.card sets := by
  induction sets generalizing used with
  | nil =>
      simp [Choices, choiceProduct]
  | cons s ss ih =>
      rw [List.pairwise_cons] at hnested
      have hus : used ⊆ s := hused s (by simp)
      simp only [Choices, Fintype.card_sigma, choiceProduct]
      rw [show (∑ x : {x // x ∈ s \ used},
          Fintype.card (Choices (insert x.1 used) ss)) =
          ∑ _x : {x // x ∈ s \ used},
            choiceProduct (used.card + 1) ss by
        apply Finset.sum_congr rfl
        intro x _
        rw [ih (insert x.1 used) hnested.2]
        · rw [Finset.card_insert_of_notMem]
          exact (Finset.mem_sdiff.mp x.2).2
        · intro t ht y hy
          rw [Finset.mem_insert] at hy
          rcases hy with rfl | hy
          · exact hnested.1 t ht (Finset.mem_sdiff.mp x.2).1
          · exact hused t (by simp [ht]) hy]
      simp [Finset.card_sdiff, Finset.inter_eq_left.mpr hus]

end A330432Lean
