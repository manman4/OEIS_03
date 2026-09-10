import A399654Lean.EdgeCover
import Mathlib.Algebra.BigOperators.Group.Finset.Basic

/-!
# The edge bound used by 399654_01.c and 399654_03.c
-/

namespace A399654Lean

open scoped BigOperators

variable {Edge : Type*} [DecidableEq Edge]

/-- An edge-minimal realization is covered by one selected cycle of every
target length and therefore has at most the sum of the target lengths edges. -/
theorem edgeMinimal_card_le_sum
    {IsCycle : Nat → Finset Edge → Prop} {H : Finset Edge} {S : Finset Nat}
    (cycle_card : ∀ L C, IsCycle L C → C.card = L)
    (hMinimal : EdgeMinimal IsCycle H S) :
    H.card ≤ ∑ L ∈ S, L := by
  classical
  let C : Nat → Finset Edge := fun L ↦
    if hLS : L ∈ S then Classical.choose ((hMinimal.1 L).2 hLS) else ∅
  have hC : ∀ L ∈ S, IsCycle L (C L) ∧ C L ⊆ H := by
    intro L hLS
    simp only [C, dif_pos hLS]
    exact Classical.choose_spec ((hMinimal.1 L).2 hLS)
  calc
    H.card ≤ (S.biUnion C).card :=
      Finset.card_le_card (subset_biUnion_selected_cycles hMinimal C hC)
    _ ≤ ∑ L ∈ S, (C L).card := Finset.card_biUnion_le
    _ = ∑ L ∈ S, L := by
      apply Finset.sum_congr rfl
      intro L hLS
      exact cycle_card L (C L) (hC L hLS).1

/-- Every finite realization has a sub-realization satisfying the edge bound.
This is the existence statement used to justify the pruning in both programs. -/
theorem exists_realization_card_le_sum
    (IsCycle : Nat → Finset Edge → Prop)
    (cycle_card : ∀ L C, IsCycle L C → C.card = L)
    (G : Finset Edge) (S : Finset Nat) (hG : Realizes IsCycle G S) :
    ∃ H : Finset Edge,
      H ⊆ G ∧ Realizes IsCycle H S ∧ H.card ≤ ∑ L ∈ S, L := by
  obtain ⟨H, hHG, hMinimal⟩ := exists_edgeMinimal_subset IsCycle G S hG
  exact ⟨H, hHG, hMinimal.1, edgeMinimal_card_le_sum cycle_card hMinimal⟩

end A399654Lean

