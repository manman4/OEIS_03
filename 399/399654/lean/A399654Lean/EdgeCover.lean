import A399654Lean.MinimalWitness
import Mathlib.Data.Finset.Union

/-!
# Covering an edge-minimal witness by selected cycles
-/

namespace A399654Lean

variable {Edge : Type*} [DecidableEq Edge]

/-- Deleting an edge from an edge-minimal realization loses some target
length, so every cycle of that length contains the deleted edge. -/
theorem exists_length_all_cycles_contain_edge
    {IsCycle : Nat → Finset Edge → Prop} {H : Finset Edge} {S : Finset Nat}
    (hMinimal : EdgeMinimal IsCycle H S) {e : Edge} (he : e ∈ H) :
    ∃ L ∈ S, ∀ C : Finset Edge, IsCycle L C → C ⊆ H → e ∈ C := by
  classical
  have hLost : ∃ L, L ∈ S ∧ ¬HasCycle IsCycle (H.erase e) L := by
    by_contra hNoLost
    apply hMinimal.2 e he
    intro L
    constructor
    · intro hCycle
      exact (hMinimal.1 L).1
        (hasCycle_mono (Finset.erase_subset e H) hCycle)
    · intro hLS
      by_contra hMissing
      exact hNoLost ⟨L, hLS, hMissing⟩
  obtain ⟨L, hLS, hLostL⟩ := hLost
  refine ⟨L, hLS, ?_⟩
  intro C hCycle hCH
  by_contra heC
  apply hLostL
  refine ⟨C, hCycle, ?_⟩
  intro x hx
  exact Finset.mem_erase.mpr ⟨by
    intro hxe
    subst x
    exact heC hx, hCH hx⟩

/-- If one cycle of every target length is selected, those cycles cover every
edge of an edge-minimal realization. -/
theorem subset_biUnion_selected_cycles
    {IsCycle : Nat → Finset Edge → Prop} {H : Finset Edge} {S : Finset Nat}
    (hMinimal : EdgeMinimal IsCycle H S) (C : Nat → Finset Edge)
    (hC : ∀ L ∈ S, IsCycle L (C L) ∧ C L ⊆ H) :
    H ⊆ S.biUnion C := by
  intro e he
  obtain ⟨L, hLS, hAll⟩ :=
    exists_length_all_cycles_contain_edge hMinimal he
  exact Finset.mem_biUnion.mpr ⟨L, hLS, hAll (C L) (hC L hLS).1 (hC L hLS).2⟩

end A399654Lean
