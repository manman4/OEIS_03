import A399654Lean.Definitions

/-!
# Edge-minimal witnesses

Every finite realization has a sub-realization from which no edge can be
deleted without changing its cycle spectrum.
-/

namespace A399654Lean

variable {Edge : Type*} [DecidableEq Edge]

theorem exists_edgeMinimal_subset
    (IsCycle : Nat → Finset Edge → Prop) (G : Finset Edge) (S : Finset Nat)
    (hG : Realizes IsCycle G S) :
    ∃ H : Finset Edge, H ⊆ G ∧ EdgeMinimal IsCycle H S := by
  classical
  induction G using Finset.strongInductionOn with
  | _ G ih =>
      by_cases hDelete : ∃ e ∈ G, Realizes IsCycle (G.erase e) S
      · obtain ⟨e, he, hErase⟩ := hDelete
        obtain ⟨H, hHG, hMinimal⟩ :=
          ih (G.erase e) (Finset.erase_ssubset he) hErase
        exact ⟨H, hHG.trans (Finset.erase_subset e G), hMinimal⟩
      · exact ⟨G, Finset.Subset.rfl, hG, by
          intro e he hErase
          exact hDelete ⟨e, he, hErase⟩⟩

end A399654Lean
