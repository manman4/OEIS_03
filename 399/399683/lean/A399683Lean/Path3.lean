import A399683Lean.Definitions

/-!
# The case `P₃`

This file formalizes the adjacent-different-edges lemma and the upper and lower
bounds corresponding to `T(n, 3) = 1`.
-/

namespace A399683Lean

universe u v

/-- Two differently colored edges in a complete graph yield two adjacent
differently colored edges, hence a rainbow `P₃`. -/
theorem rainbowP3_of_usesAtLeastTwo
    {V : Type u} {C : Type v} {χ : Coloring V C}
    (hsymm : SymmetricColoring χ) (htwo : UsesAtLeastTwo χ) :
    HasRainbowP3 χ := by
  rcases htwo with ⟨a, b, c, d, hab, hcd, hcolor⟩
  by_cases hac : a = c
  · subst c
    have hbd : b ≠ d := by
      intro h
      subst d
      exact hcolor rfl
    refine ⟨b, a, d, ⟨hab.symm, hbd, hcd⟩, ?_⟩
    simpa [hsymm b a] using hcolor
  by_cases had : a = d
  · subst d
    have hbc : b ≠ c := by
      intro h
      subst c
      exact hcolor (hsymm b a).symm
    refine ⟨b, a, c, ⟨hab.symm, hbc, hcd.symm⟩, ?_⟩
    simpa [hsymm b a, hsymm a c] using hcolor
  by_cases hbc : b = c
  · subst c
    have had' : a ≠ d := by
      intro h
      subst d
      exact hcolor (hsymm b a).symm
    exact ⟨a, b, d, ⟨hab, had', hcd⟩, hcolor⟩
  by_cases hbd : b = d
  · subst d
    have hac' : a ≠ c := by
      intro h
      subst c
      exact hcolor rfl
    refine ⟨a, b, c, ⟨hab, hac', hcd.symm⟩, ?_⟩
    simpa [hsymm c b] using hcolor
  by_cases hbridge : χ a b = χ b c
  · have hbridge_diff : χ b c ≠ χ c d := by
      intro h
      exact hcolor (hbridge.trans h)
    exact ⟨b, c, d, ⟨hbc, hbd, hcd⟩, hbridge_diff⟩
  · exact ⟨a, b, c, ⟨hab, hac, hbc⟩, hbridge⟩

/-- A symmetric coloring with no rainbow `P₃` uses at most one color on its
non-loop edges. -/
theorem no_rainbowP3_usesAtMostOne
    {V : Type u} {C : Type v} {χ : Coloring V C}
    (hsymm : SymmetricColoring χ) {a b : V} (hab : a ≠ b)
    (hno : ¬ HasRainbowP3 χ) : UsesAtMostOne χ := by
  refine ⟨χ a b, ?_⟩
  intro u v huv
  by_contra huvColor
  have htwo : UsesAtLeastTwo χ :=
    ⟨a, b, u, v, hab, huv, fun h => huvColor h.symm⟩
  exact hno (rainbowP3_of_usesAtLeastTwo hsymm htwo)

/-- The constant coloring supplies the lower bound of one color. -/
def oneColoring {V : Type u} {C : Type v} (α : C) : Coloring V C :=
  fun _ _ => α

theorem oneColoring_symmetric {V : Type u} {C : Type v} (α : C) :
    SymmetricColoring (oneColoring (V := V) α) := by
  intro a b
  rfl

theorem oneColoring_has_no_rainbowP3
    {V : Type u} {C : Type v} (α : C) :
    ¬ HasRainbowP3 (oneColoring (V := V) α) := by
  rintro ⟨a, b, c, _, hcolor⟩
  exact hcolor rfl

end A399683Lean
