import Mathlib.Tactic

/-!
# Small path anti-Ramsey definitions

The complete graph is represented by a symmetric function assigning a color to
each ordered pair of vertices.  Values on loops are irrelevant: every path
predicate below explicitly requires distinct vertices.
-/

namespace A399683Lean

universe u v

/-- An edge-coloring of a complete graph, represented on ordered vertex pairs. -/
abbrev Coloring (V : Type u) (C : Type v) := V → V → C

/-- The coloring does not depend on the orientation of an edge. -/
def SymmetricColoring {V : Type u} {C : Type v} (χ : Coloring V C) : Prop :=
  ∀ a b, χ a b = χ b a

/-- Three vertices, all pairwise distinct. -/
structure Distinct3 {V : Type u} (a b c : V) : Prop where
  ab : a ≠ b
  ac : a ≠ c
  bc : b ≠ c

/-- Four vertices, all pairwise distinct. -/
structure Distinct4 {V : Type u} (a b c d : V) : Prop where
  ab : a ≠ b
  ac : a ≠ c
  ad : a ≠ d
  bc : b ≠ c
  bd : b ≠ d
  cd : c ≠ d

instance distinct4Decidable {V : Type u} [DecidableEq V] (a b c d : V) :
    Decidable (Distinct4 a b c d) := by
  by_cases hab : a ≠ b
  · by_cases hac : a ≠ c
    · by_cases had : a ≠ d
      · by_cases hbc : b ≠ c
        · by_cases hbd : b ≠ d
          · by_cases hcd : c ≠ d
            · exact isTrue ⟨hab, hac, had, hbc, hbd, hcd⟩
            · exact isFalse (fun h => hcd h.cd)
          · exact isFalse (fun h => hbd h.bd)
        · exact isFalse (fun h => hbc h.bc)
      · exact isFalse (fun h => had h.ad)
    · exact isFalse (fun h => hac h.ac)
  · exact isFalse (fun h => hab h.ab)

/-- Five vertices, all pairwise distinct. -/
structure Distinct5 {V : Type u} (a b c x y : V) : Prop where
  ab : a ≠ b
  ac : a ≠ c
  ax : a ≠ x
  ay : a ≠ y
  bc : b ≠ c
  bx : b ≠ x
  by_ne : b ≠ y
  cx : c ≠ x
  cy : c ≠ y
  xy : x ≠ y

/-- A vertex outside a specified set of three vertices. -/
structure Outside3 {V : Type u} (a b c x : V) : Prop where
  xa : x ≠ a
  xb : x ≠ b
  xc : x ≠ c

/-- A rainbow copy of the three-vertex path. -/
def HasRainbowP3 {V : Type u} {C : Type v} (χ : Coloring V C) : Prop :=
  ∃ a b c, Distinct3 a b c ∧ χ a b ≠ χ b c

/-- A rainbow copy of the four-vertex path. -/
def HasRainbowP4 {V : Type u} {C : Type v} (χ : Coloring V C) : Prop :=
  ∃ a b c d, Distinct4 a b c d ∧
    χ a b ≠ χ b c ∧ χ a b ≠ χ c d ∧ χ b c ≠ χ c d

/-- All non-loop edges receive one fixed color. -/
def UsesAtMostOne {V : Type u} {C : Type v} (χ : Coloring V C) : Prop :=
  ∃ α, ∀ ⦃a b⦄, a ≠ b → χ a b = α

/-- All non-loop edges receive one of two fixed colors. -/
def UsesAtMostTwo {V : Type u} {C : Type v} (χ : Coloring V C) : Prop :=
  ∃ α β, ∀ ⦃a b⦄, a ≠ b → χ a b = α ∨ χ a b = β

/-- There are two edges with different colors. -/
def UsesAtLeastTwo {V : Type u} {C : Type v} (χ : Coloring V C) : Prop :=
  ∃ a b c d, a ≠ b ∧ c ≠ d ∧ χ a b ≠ χ c d

/-- There are three edges whose colors are pairwise distinct. -/
def UsesAtLeastThree {V : Type u} {C : Type v} (χ : Coloring V C) : Prop :=
  ∃ a b c d e f,
    a ≠ b ∧ c ≠ d ∧ e ≠ f ∧
    χ a b ≠ χ c d ∧ χ a b ≠ χ e f ∧ χ c d ≠ χ e f

/-- Every three distinct vertices can be extended to five distinct vertices.
For finite vertex types this is equivalent to having at least five vertices. -/
def HasAtLeastFiveVertices (V : Type u) : Prop :=
  ∀ ⦃a b c : V⦄, Distinct3 a b c →
    ∃ x y, Distinct5 a b c x y

/-- A finite type of cardinality at least five satisfies the vertex-extension
condition used in the `P₄` proof. -/
theorem hasAtLeastFiveVertices_of_card
    {V : Type u} [Fintype V] [DecidableEq V]
    (hcard : 5 ≤ Fintype.card V) : HasAtLeastFiveVertices V := by
  intro a b c habc
  let s : Finset V := {a, b, c}
  have hs : s.card = 3 := by
    simp [s, habc.ab, habc.ac, habc.bc]
  have hcompl : 1 < sᶜ.card := by
    rw [Finset.card_compl, hs]
    omega
  rcases Finset.one_lt_card.mp hcompl with ⟨x, hxs, y, hys, hxy⟩
  have hxnot : x ∉ s := by simpa using hxs
  have hynot : y ∉ s := by simpa using hys
  have hxa : x ≠ a := fun h => hxnot (by simp [s, h])
  have hxb : x ≠ b := fun h => hxnot (by simp [s, h])
  have hxc : x ≠ c := fun h => hxnot (by simp [s, h])
  have hya : y ≠ a := fun h => hynot (by simp [s, h])
  have hyb : y ≠ b := fun h => hynot (by simp [s, h])
  have hyc : y ≠ c := fun h => hynot (by simp [s, h])
  exact ⟨x, y, ⟨habc.ab, habc.ac, hxa.symm, hya.symm, habc.bc,
    hxb.symm, hyb.symm, hxc.symm, hyc.symm, hxy⟩⟩

/-- If a four-vertex path is not rainbow, two of its three edge colors agree. -/
lemma repeated_color_of_no_rainbowP4
    {V : Type u} {C : Type v} {χ : Coloring V C}
    (hχ : ¬ HasRainbowP4 χ) {a b c d : V} (h : Distinct4 a b c d) :
    χ a b = χ b c ∨ χ a b = χ c d ∨ χ b c = χ c d := by
  by_cases h₁ : χ a b = χ b c
  · exact Or.inl h₁
  by_cases h₂ : χ a b = χ c d
  · exact Or.inr (Or.inl h₂)
  by_cases h₃ : χ b c = χ c d
  · exact Or.inr (Or.inr h₃)
  exact False.elim (hχ ⟨a, b, c, d, h, h₁, h₂, h₃⟩)

/-- On a non-rainbow four-vertex path, if the last two colors are distinct,
the first color is one of them. -/
lemma first_color_eq_of_no_rainbowP4
    {V : Type u} {C : Type v} {χ : Coloring V C}
    (hχ : ¬ HasRainbowP4 χ) {a b c d : V} (h : Distinct4 a b c d)
    {α β : C} (hbc : χ b c = α) (hcd : χ c d = β) (hαβ : α ≠ β) :
    χ a b = α ∨ χ a b = β := by
  rcases repeated_color_of_no_rainbowP4 hχ h with h₁ | h₂ | h₃
  · exact Or.inl (h₁.trans hbc)
  · exact Or.inr (h₂.trans hcd)
  · exact False.elim (hαβ (hbc ▸ hcd ▸ h₃))

end A399683Lean
