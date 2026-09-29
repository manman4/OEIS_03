import A374275Lean.GoldenNontrivialFactor

/-!
# Swapping two conjugate pairs of irreducible classes
-/

namespace A374275Lean
namespace GoldenRing

open Classical

/-- Swap the conjugate pair of `p` with the conjugate pair of `q`.
The hypotheses say that the four classes are distinct. -/
noncomputable def conjugatePairSwap (p q : IrredClass)
    (hp : conjIrred p ≠ p) (hq : conjIrred q ≠ q)
    (hpq : p ≠ q) (hpcq : p ≠ conjIrred q) : IrredClass ≃ IrredClass := by
  let f : IrredClass → IrredClass := fun x ↦
    if x = p then q
    else if x = q then p
    else if x = conjIrred p then conjIrred q
    else if x = conjIrred q then conjIrred p
    else x
  have hcp_q : conjIrred p ≠ q := by
    intro h
    apply hpcq
    apply conjIrred.injective
    simpa using h
  have hcq_p : conjIrred q ≠ p := Ne.symm hpcq
  have hf : ∀ x, f (f x) = x := by
    intro x
    simp only [f]
    by_cases hxp : x = p
    · subst x
      simp [Ne.symm hpq]
    by_cases hxq : x = q
    · subst x
      simp [Ne.symm hpq]
    by_cases hxcp : x = conjIrred p
    · subst x
      simp [Ne.symm hpq, hp, hcp_q, hcq_p, hq]
    by_cases hxcq : x = conjIrred q
    · subst x
      simp [Ne.symm hpq, hp, hcp_q, hcq_p, hq]
    simp [hxp, hxq, hxcp, hxcq]
  exact
    { toFun := f
      invFun := f
      left_inv := hf
      right_inv := hf }

@[simp] lemma conjugatePairSwap_apply_p (p q : IrredClass)
    (hp : conjIrred p ≠ p) (hq : conjIrred q ≠ q)
    (hpq : p ≠ q) (hpcq : p ≠ conjIrred q) :
    conjugatePairSwap p q hp hq hpq hpcq p = q := by
  simp [conjugatePairSwap]

@[simp] lemma conjugatePairSwap_apply_q (p q : IrredClass)
    (hp : conjIrred p ≠ p) (hq : conjIrred q ≠ q)
    (hpq : p ≠ q) (hpcq : p ≠ conjIrred q) :
    conjugatePairSwap p q hp hq hpq hpcq q = p := by
  simp [conjugatePairSwap, Ne.symm hpq]

lemma conjugatePairSwap_commutes (p q : IrredClass)
    (hp : conjIrred p ≠ p) (hq : conjIrred q ≠ q)
    (hpq : p ≠ q) (hpcq : p ≠ conjIrred q) :
    CommutesConj (conjugatePairSwap p q hp hq hpq hpcq) := by
  intro x
  have hcp_q : conjIrred p ≠ q := by
    intro h
    apply hpcq
    apply conjIrred.injective
    simpa using h
  have hcq_p : conjIrred q ≠ p := Ne.symm hpcq
  simp only [conjugatePairSwap]
  by_cases hxp : x = p
  · subst x
    simp [hp, hcp_q]
  by_cases hxq : x = q
  · subst x
    simp [Ne.symm hpq, hq, hcq_p]
  by_cases hxcp : x = conjIrred p
  · subst x
    simp [hp, hcp_q]
  by_cases hxcq : x = conjIrred q
  · subst x
    simp [Ne.symm hpq, hq, hcq_p]
  have hcxp : conjIrred x ≠ p := by
    intro h
    apply hxcp
    apply conjIrred.injective
    simpa using h
  have hcxq : conjIrred x ≠ q := by
    intro h
    apply hxcq
    apply conjIrred.injective
    simpa using h
  have hcxcp : conjIrred x ≠ conjIrred p := by
    intro h
    exact hxp (conjIrred.injective h)
  have hcxcq : conjIrred x ≠ conjIrred q := by
    intro h
    exact hxq (conjIrred.injective h)
  simp [hxp, hxq, hxcp, hxcq, hcxp, hcxq, hcxcp, hcxcq]

end GoldenRing
end A374275Lean
