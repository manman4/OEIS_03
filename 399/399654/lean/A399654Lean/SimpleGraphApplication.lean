import A399654Lean.EdgeBound
import Mathlib.Combinatorics.SimpleGraph.Finite
import Mathlib.Combinatorics.SimpleGraph.Paths
import Mathlib.Data.Set.Card

/-!
# Application to finite simple graphs

A possible cycle is represented by the finset of its edges in the complete
graph on the fixed vertex type.  Containment of that finset in a graph's edge
finset is equivalent to the usual existence of a cycle walk in the graph.
-/

namespace A399654Lean

open SimpleGraph
open scoped BigOperators

variable {V : Type*} [Fintype V] [DecidableEq V]

/-- `C` is the edge set of a simple cycle of length `L` on `V`. -/
def IsSimpleCycleEdgeSet (L : Nat) (C : Finset (Sym2 V)) : Prop :=
  ∃ v : V, ∃ p : (⊤ : SimpleGraph V).Walk v v,
    p.IsCycle ∧ p.length = L ∧ p.edges.toFinset = C

omit [Fintype V] in
theorem isSimpleCycleEdgeSet_card {L : Nat} {C : Finset (Sym2 V)}
    (hC : IsSimpleCycleEdgeSet (V := V) L C) : C.card = L := by
  obtain ⟨v, p, hp, hLength, rfl⟩ := hC
  rw [List.toFinset_card_of_nodup hp.isTrail.edges_nodup]
  simpa only [SimpleGraph.Walk.length_edges] using hLength

/-- The usual statement that `G` has exactly the cycle lengths in `S`. -/
def GraphRealizes (G : SimpleGraph V) (S : Finset Nat) : Prop :=
  ∀ L : Nat,
    (∃ v : V, ∃ p : G.Walk v v, p.IsCycle ∧ p.length = L) ↔ L ∈ S

omit [Fintype V] in
theorem hasCycle_edges_iff (G : SimpleGraph V) (E : Finset (Sym2 V))
    (hE : (E : Set (Sym2 V)) = G.edgeSet) (L : Nat) :
    HasCycle (IsSimpleCycleEdgeSet (V := V)) E L ↔
      ∃ v : V, ∃ p : G.Walk v v, p.IsCycle ∧ p.length = L := by
  constructor
  · rintro ⟨C, ⟨v, p, hp, hLength, hEdges⟩, hCG⟩
    have hTransfer : ∀ e, e ∈ p.edges → e ∈ G.edgeSet := by
      intro e he
      rw [← hE]
      apply hCG
      rw [← hEdges]
      simpa using he
    let q : G.Walk v v := p.transfer G hTransfer
    refine ⟨v, q, hp.transfer hTransfer, ?_⟩
    simpa only [q, SimpleGraph.Walk.length_transfer] using hLength
  · rintro ⟨v, p, hp, hLength⟩
    have hTransfer : ∀ e, e ∈ p.edges → e ∈ (⊤ : SimpleGraph V).edgeSet := by
      intro e he
      exact SimpleGraph.edgeSet_mono le_top (p.edges_subset_edgeSet he)
    let q : (⊤ : SimpleGraph V).Walk v v := p.transfer ⊤ hTransfer
    refine ⟨p.edges.toFinset, ⟨v, q, hp.transfer hTransfer, ?_, ?_⟩, ?_⟩
    · simpa only [q, SimpleGraph.Walk.length_transfer] using hLength
    · simp only [q, SimpleGraph.Walk.edges_transfer]
    · intro e he
      have heG : e ∈ G.edgeSet :=
        p.edges_subset_edgeSet (by simpa using he)
      rw [← hE] at heG
      exact heG

theorem hasCycle_edgeFinset_iff (G : SimpleGraph V) [DecidableRel G.Adj]
    (L : Nat) :
    HasCycle (IsSimpleCycleEdgeSet (V := V)) G.edgeFinset L ↔
      ∃ v : V, ∃ p : G.Walk v v, p.IsCycle ∧ p.length = L :=
  hasCycle_edges_iff G G.edgeFinset (SimpleGraph.coe_edgeFinset G) L

omit [Fintype V] in
theorem realizes_edges_iff (G : SimpleGraph V) (E : Finset (Sym2 V))
    (hE : (E : Set (Sym2 V)) = G.edgeSet) (S : Finset Nat) :
    Realizes (IsSimpleCycleEdgeSet (V := V)) E S ↔ GraphRealizes G S := by
  constructor <;> intro h L
  · exact (hasCycle_edges_iff G E hE L).symm.trans (h L)
  · exact (hasCycle_edges_iff G E hE L).trans (h L)

theorem realizes_edgeFinset_iff (G : SimpleGraph V) [DecidableRel G.Adj]
    (S : Finset Nat) :
    Realizes (IsSimpleCycleEdgeSet (V := V)) G.edgeFinset S ↔
      GraphRealizes G S := by
  constructor <;> intro h L
  · exact (hasCycle_edgeFinset_iff G L).symm.trans (h L)
  · exact (hasCycle_edgeFinset_iff G L).trans (h L)

/-- A finite simple graph realizing `S` has a spanning subgraph with the same
cycle spectrum and at most `sum_{L in S} L` edges. -/
theorem exists_spanning_subgraph_card_le_sum
    (G : SimpleGraph V) [DecidableRel G.Adj] (S : Finset Nat)
    (hG : GraphRealizes G S) :
    ∃ H : SimpleGraph V,
      H ≤ G ∧ GraphRealizes H S ∧ H.edgeSet.ncard ≤ ∑ L ∈ S, L := by
  classical
  have hG' : Realizes (IsSimpleCycleEdgeSet (V := V)) G.edgeFinset S :=
    (realizes_edgeFinset_iff G S).2 hG
  obtain ⟨E, hEG, hERealizes, hECard⟩ :=
    exists_realization_card_le_sum
      (IsSimpleCycleEdgeSet (V := V))
      (fun _ _ hC ↦ isSimpleCycleEdgeSet_card hC)
      G.edgeFinset S hG'
  let H : SimpleGraph V := SimpleGraph.fromEdgeSet (E : Set (Sym2 V))
  have hHEdges : H.edgeSet = (E : Set (Sym2 V)) := by
    change (SimpleGraph.fromEdgeSet (E : Set (Sym2 V))).edgeSet = _
    rw [SimpleGraph.edgeSet_fromEdgeSet]
    rw [sdiff_eq_left]
    rw [Set.disjoint_left]
    intro e heE heDiag
    have heG : e ∈ G.edgeFinset := hEG heE
    exact G.not_isDiag_of_mem_edgeFinset heG
      ((Sym2.mem_diagSet_iff_isDiag e).mp heDiag)
  refine ⟨H, ?_, ?_, ?_⟩
  · apply SimpleGraph.edgeSet_subset_edgeSet.mp
    intro e he
    rw [hHEdges] at he
    exact SimpleGraph.mem_edgeFinset.mp (hEG he)
  · exact (realizes_edges_iff H E hHEdges.symm S).1 hERealizes
  · rw [hHEdges]
    simpa using hECard

end A399654Lean
