import A399654Lean.Main

/-!
Run `lake env lean A399654Lean/AxiomAudit.lean` to display the axiom
dependencies of the public results.
-/

namespace A399654Lean

#print axioms hasCycle_mono
#print axioms exists_edgeMinimal_subset
#print axioms exists_length_all_cycles_contain_edge
#print axioms subset_biUnion_selected_cycles
#print axioms edgeMinimal_card_le_sum
#print axioms exists_realization_card_le_sum
#print axioms isSimpleCycleEdgeSet_card
#print axioms hasCycle_edges_iff
#print axioms hasCycle_edgeFinset_iff
#print axioms realizes_edges_iff
#print axioms realizes_edgeFinset_iff
#print axioms exists_spanning_subgraph_card_le_sum

end A399654Lean
