import Mathlib.Data.Finset.Card

/-!
# Cycle spectra as a finite set system

The edge-bound argument for A399654 only uses the edge set of each cycle.
`IsCycle L C` says that `C` is the edge set of a cycle of length `L`.
-/

namespace A399654Lean

variable {Edge : Type*} [DecidableEq Edge]

/-- `H` contains a cycle of length `L`. -/
def HasCycle (IsCycle : Nat → Finset Edge → Prop)
    (H : Finset Edge) (L : Nat) : Prop :=
  ∃ C : Finset Edge, IsCycle L C ∧ C ⊆ H

/-- The cycle-length spectrum of an edge set. -/
def Realizes (IsCycle : Nat → Finset Edge → Prop)
    (H : Finset Edge) (S : Finset Nat) : Prop :=
  ∀ L : Nat, HasCycle IsCycle H L ↔ L ∈ S

/-- No single edge can be deleted while preserving the spectrum `S`. -/
def EdgeMinimal (IsCycle : Nat → Finset Edge → Prop)
    (H : Finset Edge) (S : Finset Nat) : Prop :=
  Realizes IsCycle H S ∧
    ∀ e ∈ H, ¬Realizes IsCycle (H.erase e) S

omit [DecidableEq Edge] in
theorem hasCycle_mono {IsCycle : Nat → Finset Edge → Prop}
    {H K : Finset Edge} (hHK : H ⊆ K) {L : Nat}
    (h : HasCycle IsCycle H L) : HasCycle IsCycle K L := by
  obtain ⟨C, hC, hCH⟩ := h
  exact ⟨C, hC, hCH.trans hHK⟩

end A399654Lean
