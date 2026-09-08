import A399683Lean.Definitions

/-!
# Two-color lower bound for `P₄`

The construction uses one exceptional edge and one color for every other edge.
-/

namespace A399683Lean

universe u v

/-- Three edges colored with at most two colors cannot be pairwise distinct. -/
theorem atMostTwo_has_no_rainbowP4
    {V : Type u} {C : Type v} {χ : Coloring V C}
    (htwo : UsesAtMostTwo χ) : ¬ HasRainbowP4 χ := by
  rcases htwo with ⟨α, β, hpalette⟩
  rintro ⟨a, b, c, d, hvertices, h₁₂, h₁₃, h₂₃⟩
  have hab := hpalette hvertices.ab
  have hbc := hpalette hvertices.bc
  have hcd := hpalette hvertices.cd
  aesop

/-- Color the edge `{p,q}` with `true` and all other edges with `false`. -/
def exceptionalEdgeColoring {V : Type u} [DecidableEq V] (p q : V) : Coloring V Bool :=
  fun a b => (a = p ∧ b = q) ∨ (a = q ∧ b = p)

theorem exceptionalEdgeColoring_symmetric
    {V : Type u} [DecidableEq V] (p q : V) :
    SymmetricColoring (exceptionalEdgeColoring p q) := by
  intro a b
  simp only [exceptionalEdgeColoring]
  by_cases hap : a = p <;> by_cases haq : a = q <;>
    by_cases hbp : b = p <;> by_cases hbq : b = q <;> simp_all

theorem exceptionalEdgeColoring_usesAtMostTwo
    {V : Type u} [DecidableEq V] (p q : V) :
    UsesAtMostTwo (exceptionalEdgeColoring p q) := by
  refine ⟨false, true, ?_⟩
  intro a b _
  cases exceptionalEdgeColoring p q a b <;> simp

/-- With three distinct vertices, the exceptional-edge construction really
uses both colors. -/
theorem exceptionalEdgeColoring_usesAtLeastTwo
    {V : Type u} [DecidableEq V] {p q r : V} (h : Distinct3 p q r) :
    UsesAtLeastTwo (exceptionalEdgeColoring p q) := by
  refine ⟨p, q, p, r, h.ab, h.ac, ?_⟩
  have hrq : r ≠ q := h.bc.symm
  simp [exceptionalEdgeColoring, h.ab, hrq]

theorem exceptionalEdgeColoring_has_no_rainbowP4
    {V : Type u} [DecidableEq V] (p q : V) :
    ¬ HasRainbowP4 (exceptionalEdgeColoring p q) :=
  atMostTwo_has_no_rainbowP4 (exceptionalEdgeColoring_usesAtMostTwo p q)

end A399683Lean
