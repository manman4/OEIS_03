import A399683Lean.Main

/-!
Run `lake env lean A399683Lean/AxiomAudit.lean` to display the axiom
dependencies of the public results.
-/

namespace A399683Lean

#print axioms rainbowP3_of_usesAtLeastTwo
#print axioms no_rainbowP3_usesAtMostOne
#print axioms oneColoring_has_no_rainbowP3
#print axioms hasAtLeastFiveVertices_of_card
#print axioms rainbowP4_of_usesAtLeastThree
#print axioms no_rainbowP4_not_usesAtLeastThree
#print axioms finite_no_rainbowP4_not_usesAtLeastThree
#print axioms atMostTwo_has_no_rainbowP4
#print axioms exceptionalEdgeColoring_usesAtLeastTwo
#print axioms exceptionalEdgeColoring_has_no_rainbowP4
#print axioms k4_rainbowP4_of_usesAtLeastFour
#print axioms k4_no_rainbowP4_not_usesAtLeastFour
#print axioms k4MatchingColoring_usesAtLeastThree
#print axioms k4MatchingColoring_has_no_rainbowP4

end A399683Lean
