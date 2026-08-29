import A330432Lean.Main

/-!
Run `lake env lean A330432Lean/AxiomAudit.lean` to display the axiom
dependencies of the public results.
-/

namespace A330432Lean

#print axioms card_choices_eq_choiceProduct
#print axioms exists_choices_of_isChoiceList
#print axioms A330432Choice.toList_perm
#print axioms A330432Permutation.exists_choice
#print axioms A330432Choice.toPermutation_injective
#print axioms A330432Choice.toPermutation_surjective
#print axioms choiceEquivA330432Permutation
#print axioms card_a330432Choice_eq_product
#print axioms a330432Permutation_card_eq_product

end A330432Lean
