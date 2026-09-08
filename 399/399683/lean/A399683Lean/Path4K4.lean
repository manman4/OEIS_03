import A399683Lean.Definitions

/-!
# The exceptional case `T(4,4) = 3`

The six edges of `K₄` are indexed explicitly.  A small kernel-checked finite
lemma says that any four distinct edges contain the three edges of a spanning
path.  No anti-Ramsey formula is used.
-/

namespace A399683Lean

universe v

inductive K4Edge where
  | e01 | e02 | e03 | e12 | e13 | e23
  deriving DecidableEq, Fintype

def k4EdgeOf (a b : Fin 4) : K4Edge :=
  if (a = 0 ∧ b = 1) ∨ (a = 1 ∧ b = 0) then .e01
  else if (a = 0 ∧ b = 2) ∨ (a = 2 ∧ b = 0) then .e02
  else if (a = 0 ∧ b = 3) ∨ (a = 3 ∧ b = 0) then .e03
  else if (a = 1 ∧ b = 2) ∨ (a = 2 ∧ b = 1) then .e12
  else if (a = 1 ∧ b = 3) ∨ (a = 3 ∧ b = 1) then .e13
  else .e23

def k4EdgeColor {C : Type v} (χ : Coloring (Fin 4) C) : K4Edge → C
  | .e01 => χ 0 1
  | .e02 => χ 0 2
  | .e03 => χ 0 3
  | .e12 => χ 1 2
  | .e13 => χ 1 3
  | .e23 => χ 2 3

lemma k4EdgeColor_edgeOf {C : Type v} {χ : Coloring (Fin 4) C}
    (hsymm : SymmetricColoring χ) {a b : Fin 4} (hab : a ≠ b) :
    k4EdgeColor χ (k4EdgeOf a b) = χ a b := by
  have hs := hsymm a b
  fin_cases a <;> fin_cases b <;>
    simp_all [k4EdgeOf, k4EdgeColor]

def Among4 (e e₁ e₂ e₃ e₄ : K4Edge) : Prop :=
  e = e₁ ∨ e = e₂ ∨ e = e₃ ∨ e = e₄

instance among4Decidable (e e₁ e₂ e₃ e₄ : K4Edge) :
    Decidable (Among4 e e₁ e₂ e₃ e₄) := by
  unfold Among4
  infer_instance

set_option maxHeartbeats 500000 in
private theorem four_distinct_k4Edges_contain_path
    (e₁ e₂ e₃ e₄ : K4Edge)
    (h₁₂ : e₁ ≠ e₂) (h₁₃ : e₁ ≠ e₃) (h₁₄ : e₁ ≠ e₄)
    (h₂₃ : e₂ ≠ e₃) (h₂₄ : e₂ ≠ e₄) (h₃₄ : e₃ ≠ e₄) :
    ∃ a b c d : Fin 4, Distinct4 a b c d ∧
      Among4 (k4EdgeOf a b) e₁ e₂ e₃ e₄ ∧
      Among4 (k4EdgeOf b c) e₁ e₂ e₃ e₄ ∧
      Among4 (k4EdgeOf c d) e₁ e₂ e₃ e₄ := by
  fin_cases e₁ <;>
    fin_cases e₂ <;> simp_all <;>
    fin_cases e₃ <;> simp_all <;>
    fin_cases e₄ <;> simp_all
  all_goals first
    | exact ⟨0, 1, 2, 3, by decide⟩
    | exact ⟨0, 1, 3, 2, by decide⟩
    | exact ⟨0, 2, 1, 3, by decide⟩
    | exact ⟨0, 2, 3, 1, by decide⟩
    | exact ⟨0, 3, 1, 2, by decide⟩
    | exact ⟨0, 3, 2, 1, by decide⟩
    | exact ⟨1, 0, 2, 3, by decide⟩
    | exact ⟨1, 0, 3, 2, by decide⟩
    | exact ⟨1, 2, 0, 3, by decide⟩
    | exact ⟨1, 3, 0, 2, by decide⟩

/-- Four pairwise distinct colors occur on four of the six edges of `K₄`. -/
def K4UsesAtLeastFour {C : Type v} (χ : Coloring (Fin 4) C) : Prop :=
  ∃ e₁ e₂ e₃ e₄ : K4Edge,
    k4EdgeColor χ e₁ ≠ k4EdgeColor χ e₂ ∧
    k4EdgeColor χ e₁ ≠ k4EdgeColor χ e₃ ∧
    k4EdgeColor χ e₁ ≠ k4EdgeColor χ e₄ ∧
    k4EdgeColor χ e₂ ≠ k4EdgeColor χ e₃ ∧
    k4EdgeColor χ e₂ ≠ k4EdgeColor χ e₄ ∧
    k4EdgeColor χ e₃ ≠ k4EdgeColor χ e₄

private lemma color_ne_of_among4
    {C : Type v} {χ : Coloring (Fin 4) C} {e f e₁ e₂ e₃ e₄ : K4Edge}
    (hcolors :
      k4EdgeColor χ e₁ ≠ k4EdgeColor χ e₂ ∧
      k4EdgeColor χ e₁ ≠ k4EdgeColor χ e₃ ∧
      k4EdgeColor χ e₁ ≠ k4EdgeColor χ e₄ ∧
      k4EdgeColor χ e₂ ≠ k4EdgeColor χ e₃ ∧
      k4EdgeColor χ e₂ ≠ k4EdgeColor χ e₄ ∧
      k4EdgeColor χ e₃ ≠ k4EdgeColor χ e₄)
    (he : Among4 e e₁ e₂ e₃ e₄)
    (hf : Among4 f e₁ e₂ e₃ e₄) (hef : e ≠ f) :
    k4EdgeColor χ e ≠ k4EdgeColor χ f := by
  rcases he with rfl | rfl | rfl | rfl <;>
    rcases hf with rfl | rfl | rfl | rfl <;> aesop

private lemma k4_path_edges_pairwise_distinct
    {a b c d : Fin 4} (h : Distinct4 a b c d) :
    k4EdgeOf a b ≠ k4EdgeOf b c ∧
    k4EdgeOf a b ≠ k4EdgeOf c d ∧
    k4EdgeOf b c ≠ k4EdgeOf c d := by
  rcases h with ⟨hab, hac, had, hbc, hbd, hcd⟩
  fin_cases a <;> fin_cases b <;> fin_cases c <;> fin_cases d <;>
    simp_all [k4EdgeOf]

/-- The upper bound in `T(4,4) = 3`: four used colors force a rainbow `P₄`. -/
theorem k4_rainbowP4_of_usesAtLeastFour
    {C : Type v} {χ : Coloring (Fin 4) C} (hsymm : SymmetricColoring χ)
    (hfour : K4UsesAtLeastFour χ) : HasRainbowP4 χ := by
  rcases hfour with ⟨e₁, e₂, e₃, e₄, hcolors⟩
  have h₁₂ : e₁ ≠ e₂ := fun h => hcolors.1 (congrArg (k4EdgeColor χ) h)
  have h₁₃ : e₁ ≠ e₃ := fun h => hcolors.2.1 (congrArg (k4EdgeColor χ) h)
  have h₁₄ : e₁ ≠ e₄ := fun h => hcolors.2.2.1 (congrArg (k4EdgeColor χ) h)
  have h₂₃ : e₂ ≠ e₃ := fun h => hcolors.2.2.2.1 (congrArg (k4EdgeColor χ) h)
  have h₂₄ : e₂ ≠ e₄ := fun h => hcolors.2.2.2.2.1 (congrArg (k4EdgeColor χ) h)
  have h₃₄ : e₃ ≠ e₄ := fun h => hcolors.2.2.2.2.2 (congrArg (k4EdgeColor χ) h)
  rcases four_distinct_k4Edges_contain_path e₁ e₂ e₃ e₄
      h₁₂ h₁₃ h₁₄ h₂₃ h₂₄ h₃₄ with
    ⟨a, b, c, d, hvertices, hab, hbc, hcd⟩
  rcases k4_path_edges_pairwise_distinct hvertices with
    ⟨hedge_ab_bc, hedge_ab_cd, hedge_bc_cd⟩
  refine ⟨a, b, c, d, hvertices, ?_, ?_, ?_⟩
  · simpa only [k4EdgeColor_edgeOf hsymm hvertices.ab,
      k4EdgeColor_edgeOf hsymm hvertices.bc] using
      color_ne_of_among4 hcolors hab hbc hedge_ab_bc
  · simpa only [k4EdgeColor_edgeOf hsymm hvertices.ab,
      k4EdgeColor_edgeOf hsymm hvertices.cd] using
      color_ne_of_among4 hcolors hab hcd hedge_ab_cd
  · simpa only [k4EdgeColor_edgeOf hsymm hvertices.bc,
      k4EdgeColor_edgeOf hsymm hvertices.cd] using
      color_ne_of_among4 hcolors hbc hcd hedge_bc_cd

theorem k4_no_rainbowP4_not_usesAtLeastFour
    {C : Type v} {χ : Coloring (Fin 4) C} (hsymm : SymmetricColoring χ)
    (hno : ¬ HasRainbowP4 χ) : ¬ K4UsesAtLeastFour χ := by
  intro hfour
  exact hno (k4_rainbowP4_of_usesAtLeastFour hsymm hfour)

/-- The three perfect matchings of `K₄` receive three different colors. -/
def k4MatchingColoring (a b : Fin 4) : Fin 3 :=
  match k4EdgeOf a b with
  | .e01 | .e23 => 0
  | .e02 | .e13 => 1
  | .e03 | .e12 => 2

theorem k4MatchingColoring_symmetric : SymmetricColoring k4MatchingColoring := by
  intro a b
  fin_cases a <;> fin_cases b <;> decide

theorem k4MatchingColoring_usesAtLeastThree :
    UsesAtLeastThree k4MatchingColoring := by
  refine ⟨(0 : Fin 4), 1, 0, 2, 0, 3, by decide, by decide, by decide, ?_⟩
  decide

private lemma k4MatchingColoring_opposite_edges_same
    {a b c d : Fin 4} (h : Distinct4 a b c d) :
    k4MatchingColoring a b = k4MatchingColoring c d := by
  rcases h with ⟨hab, hac, had, hbc, hbd, hcd⟩
  fin_cases a <;> fin_cases b <;> fin_cases c <;> fin_cases d <;>
    simp_all [k4MatchingColoring, k4EdgeOf]

theorem k4MatchingColoring_has_no_rainbowP4 :
    ¬ HasRainbowP4 k4MatchingColoring := by
  rintro ⟨a, b, c, d, hvertices, h₁₂, h₁₃, h₂₃⟩
  exact h₁₃ (k4MatchingColoring_opposite_edges_same hvertices)

end A399683Lean
