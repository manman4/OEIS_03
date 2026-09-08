import A399683Lean.Path3

/-!
# The case `P₄` on at least five vertices

This file follows the elementary case analysis in `PATH_SMALL_CASES_JA.md`.
It proves that three used colors force a rainbow `P₄` whenever every three
distinct vertices can be extended to five distinct vertices.
-/

namespace A399683Lean

universe u v

private lemma colors_to_outside
    {V : Type u} {C : Type v} {χ : Coloring V C}
    (hsymm : SymmetricColoring χ) (hno : ¬ HasRainbowP4 χ)
    {a b c z : V} (habc : Distinct3 a b c) (hz : Outside3 a b c z)
    (hdiff : χ a b ≠ χ b c) :
    (χ a z = χ a b ∨ χ a z = χ b c) ∧
      (χ c z = χ a b ∨ χ c z = χ b c) := by
  have h₁ : Distinct4 z a b c :=
    ⟨hz.xa, hz.xb, hz.xc, habc.ab, habc.ac, habc.bc⟩
  have h₂ : Distinct4 z c b a :=
    ⟨hz.xc, hz.xb, hz.xa, habc.bc.symm, habc.ac.symm, habc.ab.symm⟩
  have ha : χ z a = χ a b ∨ χ z a = χ b c :=
    first_color_eq_of_no_rainbowP4 hno h₁ rfl rfl hdiff
  have hc : χ z c = χ b c ∨ χ z c = χ a b :=
    first_color_eq_of_no_rainbowP4 hno h₂
      (hsymm c b) (hsymm b a) hdiff.symm
  constructor
  · rcases ha with ha | ha
    · exact Or.inl ((hsymm a z).trans ha)
    · exact Or.inr ((hsymm a z).trans ha)
  · rcases hc with hc | hc
    · exact Or.inr ((hsymm c z).trans hc)
    · exact Or.inl ((hsymm c z).trans hc)

private lemma force_second₁ {D : Type v} {A B G X : D}
    (hAB : A ≠ B) (hGA : G ≠ A) (hGB : G ≠ B)
    (hbase : X = A ∨ X = B) (hpath : X = G ∨ X = B ∨ G = B) : X = B := by
  aesop

private lemma force_first₁ {D : Type v} {A B G X : D}
    (hAB : A ≠ B) (hGA : G ≠ A) (hGB : G ≠ B)
    (hbase : X = A ∨ X = B) (hpath : A = G ∨ A = X ∨ G = X) : X = A := by
  aesop

private lemma force_second₂ {D : Type v} {A B G X : D}
    (hAB : A ≠ B) (hGA : G ≠ A) (hGB : G ≠ B)
    (hbase : X = A ∨ X = B) (hpath : B = G ∨ B = X ∨ G = X) : X = B := by
  aesop

private lemma force_second₃ {D : Type v} {A B G X : D}
    (hAB : A ≠ B) (hGA : G ≠ A) (hGB : G ≠ B)
    (hbase : X = A ∨ X = B) (hpath : X = B ∨ X = G ∨ B = G) : X = B := by
  aesop

private lemma force_first₂ {D : Type v} {A B G X : D}
    (hAB : A ≠ B) (hGA : G ≠ A) (hGB : G ≠ B)
    (hbase : X = A ∨ X = B) (hpath : X = A ∨ X = G ∨ A = G) : X = A := by
  aesop

private lemma force_second_from_two_paths {D : Type v} {A B G X : D}
    (hAB : A ≠ B) (hGA : G ≠ A) (hGB : G ≠ B)
    (h₁ : B = X ∨ B = A ∨ X = A)
    (h₂ : X = B ∨ X = G ∨ B = G) : X = B := by
  aesop

private lemma force_third_from_two_paths {D : Type v} {A B G X : D}
    (hAB : A ≠ B) (hGA : G ≠ A) (hGB : G ≠ B)
    (hA : A = X ∨ A = G ∨ X = G)
    (hB : B = X ∨ B = G ∨ X = G) : X = G := by
  aesop

/-- On at least five vertices, three used colors force a rainbow `P₄`. -/
theorem rainbowP4_of_usesAtLeastThree
    {V : Type u} {C : Type v} {χ : Coloring V C}
    (hsymm : SymmetricColoring χ) (hfive : HasAtLeastFiveVertices V)
    (hthree : UsesAtLeastThree χ) : HasRainbowP4 χ := by
  by_contra hno
  rcases hthree with
    ⟨r, s, t, u, v, w, hrs, htu, hvw, h₁₂, h₁₃, h₂₃⟩
  have htwo : UsesAtLeastTwo χ := ⟨r, s, t, u, hrs, htu, h₁₂⟩
  rcases rainbowP3_of_usesAtLeastTwo hsymm htwo with
    ⟨a, b, c, habc, hab_ne_hbc⟩
  rcases hfive habc with ⟨x, y, hfive'⟩
  have hx : Outside3 a b c x :=
    ⟨hfive'.ax.symm, hfive'.bx.symm, hfive'.cx.symm⟩
  have hy : Outside3 a b c y :=
    ⟨hfive'.ay.symm, hfive'.by_ne.symm, hfive'.cy.symm⟩
  have hbase (z : V) (hz : Outside3 a b c z) :
      (χ a z = χ a b ∨ χ a z = χ b c) ∧
        (χ c z = χ a b ∨ χ c z = χ b c) :=
    colors_to_outside hsymm hno habc hz hab_ne_hbc

  have hthird : ∃ p q, p ≠ q ∧ χ p q ≠ χ a b ∧ χ p q ≠ χ b c := by
    by_cases h₁a : χ r s = χ a b
    · by_cases h₂b : χ t u = χ b c
      · refine ⟨v, w, hvw, ?_, ?_⟩
        · intro h
          exact h₁₃ (h₁a.trans h.symm)
        · intro h
          exact h₂₃ (h₂b.trans h.symm)
      · refine ⟨t, u, htu, ?_, h₂b⟩
        intro h
        exact h₁₂ (h₁a.trans h.symm)
    · by_cases h₁b : χ r s = χ b c
      · by_cases h₂a : χ t u = χ a b
        · refine ⟨v, w, hvw, ?_, ?_⟩
          · intro h
            exact h₂₃ (h₂a.trans h.symm)
          · intro h
            exact h₁₃ (h₁b.trans h.symm)
        · refine ⟨t, u, htu, h₂a, ?_⟩
          intro h
          exact h₁₂ (h₁b.trans h.symm)
      · exact ⟨r, s, hrs, h₁a, h₁b⟩
  rcases hthird with ⟨p, q, hpq, hpq_a, hpq_b⟩

  have hac_false (hac : χ a c = χ p q) : False := by
    have h_ac_colors (z : V) (hz : Outside3 a b c z) :
        χ a z = χ b c ∧ χ c z = χ a b := by
      have hb := hbase z hz
      have hpa : Distinct4 z a c b :=
        ⟨hz.xa, hz.xc, hz.xb, habc.ac, habc.ab, habc.bc.symm⟩
      have hpc : Distinct4 b a c z :=
        ⟨habc.ab.symm, habc.bc, hz.xb.symm, habc.ac, hz.xa.symm, hz.xc.symm⟩
      have hra := repeated_color_of_no_rainbowP4 hno hpa
      have hrc := repeated_color_of_no_rainbowP4 hno hpc
      rw [hsymm z a, hac, hsymm c b] at hra
      rw [hsymm b a, hac] at hrc
      exact ⟨force_second₁ hab_ne_hbc hpq_a hpq_b hb.1 hra,
        force_first₁ hab_ne_hbc hpq_a hpq_b hb.2 hrc⟩
    have hxc := h_ac_colors x hx
    have hyc := h_ac_colors y hy
    have hp₁ : Distinct4 a x y c :=
      ⟨hfive'.ax, hfive'.ay, habc.ac, hfive'.xy,
        hfive'.cx.symm, hfive'.cy.symm⟩
    have hp₂ : Distinct4 x y a c :=
      ⟨hfive'.xy, hfive'.ax.symm, hfive'.cx.symm,
        hfive'.ay.symm, hfive'.cy.symm, habc.ac⟩
    have hr₁ := repeated_color_of_no_rainbowP4 hno hp₁
    have hr₂ := repeated_color_of_no_rainbowP4 hno hp₂
    rw [hxc.1, hsymm y c, hyc.2] at hr₁
    rw [hsymm y a, hyc.1, hac] at hr₂
    have hxy : χ x y = χ b c :=
      force_second_from_two_paths hab_ne_hbc hpq_a hpq_b hr₁ hr₂
    apply hno
    refine ⟨a, c, y, x,
      ⟨habc.ac, hfive'.ay, hfive'.ax, hfive'.cy,
        hfive'.cx, hfive'.xy.symm⟩, ?_, ?_, ?_⟩
    · rw [hac, hyc.2]
      exact hpq_a
    · rw [hac, hsymm y x, hxy]
      exact hpq_b
    · rw [hyc.2, hsymm y x, hxy]
      exact hab_ne_hbc

  have hbx_false (z : V) (hz : Outside3 a b c z)
      (hbz : χ b z = χ p q) : False := by
    have hzbase := hbase z hz
    have hp₁ : Distinct4 a b z c :=
      ⟨habc.ab, hz.xa.symm, habc.ac, hz.xb.symm,
        habc.bc, hz.xc⟩
    have hp₂ : Distinct4 c b z a :=
      ⟨habc.bc.symm, hz.xc.symm, habc.ac.symm, hz.xb.symm,
        habc.ab.symm, hz.xa⟩
    have hr₁ := repeated_color_of_no_rainbowP4 hno hp₁
    have hr₂ := repeated_color_of_no_rainbowP4 hno hp₂
    rw [hbz, hsymm z c] at hr₁
    rw [hsymm c b, hbz, hsymm z a] at hr₂
    have hcz : χ c z = χ a b :=
      force_first₁ hab_ne_hbc hpq_a hpq_b hzbase.2 hr₁
    have haz : χ a z = χ b c :=
      force_second₂ hab_ne_hbc hpq_a hpq_b hzbase.1 hr₂
    have hw : ∃ z', Outside3 a b c z' ∧ z' ≠ z := by
      by_cases hzx : z = x
      · subst z
        exact ⟨y, hy, hfive'.xy.symm⟩
      · exact ⟨x, hx, Ne.symm hzx⟩
    rcases hw with ⟨z', hz', hz'z⟩
    have hz'base := hbase z' hz'
    have hp₃ : Distinct4 z' a z b :=
      ⟨hz'.xa, hz'z, hz'.xb, hz.xa.symm, habc.ab, hz.xb⟩
    have hp₄ : Distinct4 z' c z b :=
      ⟨hz'.xc, hz'z, hz'.xb, hz.xc.symm, habc.bc.symm, hz.xb⟩
    have hr₃ := repeated_color_of_no_rainbowP4 hno hp₃
    have hr₄ := repeated_color_of_no_rainbowP4 hno hp₄
    rw [hsymm z' a, haz, hsymm z b, hbz] at hr₃
    rw [hsymm z' c, hcz, hsymm z b, hbz] at hr₄
    have haz' : χ a z' = χ b c :=
      force_second₃ hab_ne_hbc hpq_a hpq_b hz'base.1 hr₃
    have hcz' : χ c z' = χ a b :=
      force_first₂ hab_ne_hbc hpq_a hpq_b hz'base.2 hr₄
    have hp₅ : Distinct4 a z' z b :=
      ⟨hz'.xa.symm, hz.xa.symm, habc.ab, hz'z, hz'.xb, hz.xb⟩
    have hp₆ : Distinct4 c z' z b :=
      ⟨hz'.xc.symm, hz.xc.symm, habc.bc.symm, hz'z, hz'.xb, hz.xb⟩
    have hr₅ := repeated_color_of_no_rainbowP4 hno hp₅
    have hr₆ := repeated_color_of_no_rainbowP4 hno hp₆
    rw [haz', hsymm z b, hbz] at hr₅
    rw [hcz', hsymm z b, hbz] at hr₆
    have hz'z_color : χ z' z = χ p q :=
      force_third_from_two_paths hab_ne_hbc hpq_a hpq_b hr₆ hr₅
    apply hno
    refine ⟨a, z, z', c,
      ⟨hz.xa.symm, hz'.xa.symm, habc.ac, hz'z.symm,
        hz.xc, hz'.xc⟩, ?_, ?_, ?_⟩
    · rw [haz, hsymm z z', hz'z_color]
      exact hpq_b.symm
    · rw [haz, hsymm z' c, hcz']
      exact hab_ne_hbc.symm
    · rw [hsymm z z', hz'z_color, hsymm z' c, hcz']
      exact hpq_a

  have hxy_false (z z' : V) (hz : Outside3 a b c z)
      (hz' : Outside3 a b c z') (hzz' : z ≠ z')
      (hcolor : χ z z' = χ p q) : False := by
    have hp₁ : Distinct4 a b z z' :=
      ⟨habc.ab, hz.xa.symm, hz'.xa.symm, hz.xb.symm,
        hz'.xb.symm, hzz'⟩
    have hp₂ : Distinct4 c b z z' :=
      ⟨habc.bc.symm, hz.xc.symm, hz'.xc.symm, hz.xb.symm,
        hz'.xb.symm, hzz'⟩
    have hr₁ := repeated_color_of_no_rainbowP4 hno hp₁
    have hr₂ := repeated_color_of_no_rainbowP4 hno hp₂
    rw [hcolor] at hr₁
    rw [hsymm c b, hcolor] at hr₂
    have hbz : χ b z = χ p q :=
      force_third_from_two_paths hab_ne_hbc hpq_a hpq_b hr₁ hr₂
    exact hbx_false z hz hbz

  by_cases hpa : p = a
  · subst p
    by_cases hqa : q = a
    · exact hpq hqa.symm
    by_cases hqb : q = b
    · subst q
      exact hpq_a rfl
    by_cases hqc : q = c
    · subst q
      exact hac_false rfl
    have hqout : Outside3 a b c q := ⟨hqa, hqb, hqc⟩
    rcases (hbase q hqout).1 with h | h
    · exact hpq_a h
    · exact hpq_b h
  by_cases hpb : p = b
  · subst p
    by_cases hqa : q = a
    · subst q
      exact hpq_a (hsymm b a)
    by_cases hqb : q = b
    · exact hpq hqb.symm
    by_cases hqc : q = c
    · subst q
      exact hpq_b rfl
    exact hbx_false q ⟨hqa, hqb, hqc⟩ rfl
  by_cases hpc : p = c
  · subst p
    by_cases hqa : q = a
    · subst q
      exact hac_false (hsymm a c)
    by_cases hqb : q = b
    · subst q
      exact hpq_b (hsymm c b)
    by_cases hqc : q = c
    · exact hpq hqc.symm
    have hqout : Outside3 a b c q := ⟨hqa, hqb, hqc⟩
    rcases (hbase q hqout).2 with h | h
    · exact hpq_a h
    · exact hpq_b h
  have hpout : Outside3 a b c p := ⟨hpa, hpb, hpc⟩
  by_cases hqa : q = a
  · subst q
    rcases (hbase p hpout).1 with h | h
    · exact hpq_a ((hsymm p a).trans h)
    · exact hpq_b ((hsymm p a).trans h)
  by_cases hqb : q = b
  · subst q
    exact hbx_false p hpout (hsymm b p)
  by_cases hqc : q = c
  · subst q
    rcases (hbase p hpout).2 with h | h
    · exact hpq_a ((hsymm p c).trans h)
    · exact hpq_b ((hsymm p c).trans h)
  exact hxy_false p q hpout ⟨hqa, hqb, hqc⟩ hpq rfl

/-- Contrapositive form of the upper bound `T(n,4) ≤ 2` for `n ≥ 5`. -/
theorem no_rainbowP4_not_usesAtLeastThree
    {V : Type u} {C : Type v} {χ : Coloring V C}
    (hsymm : SymmetricColoring χ) (hfive : HasAtLeastFiveVertices V)
    (hno : ¬ HasRainbowP4 χ) : ¬ UsesAtLeastThree χ := by
  intro hthree
  exact hno (rainbowP4_of_usesAtLeastThree hsymm hfive hthree)

end A399683Lean
