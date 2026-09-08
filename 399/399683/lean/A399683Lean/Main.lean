import A399683Lean.Path3
import A399683Lean.Path4K4
import A399683Lean.Path4Large
import A399683Lean.Path4Lower

/-!
# Small path anti-Ramsey theorems for A399683

This module assembles the elementary proofs for `k = 3, 4`.  The result for
`k = 4` on at least five vertices is also stated directly for a finite vertex
type of cardinality at least five.
-/

namespace A399683Lean

universe u v

/-- For a finite complete graph on at least five vertices, a symmetric
coloring with no rainbow `P₄` cannot use three pairwise distinct colors. -/
theorem finite_no_rainbowP4_not_usesAtLeastThree
    {V : Type u} {C : Type v} [Fintype V] [DecidableEq V]
    {χ : Coloring V C} (hsymm : SymmetricColoring χ)
    (hcard : 5 ≤ Fintype.card V) (hno : ¬ HasRainbowP4 χ) :
    ¬ UsesAtLeastThree χ :=
  no_rainbowP4_not_usesAtLeastThree hsymm
    (hasAtLeastFiveVertices_of_card hcard) hno

end A399683Lean
