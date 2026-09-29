import A374275Lean.GoldenNormCriterion

/-!
# Transporting norm classes through irreducible-factor permutations

An equivalence of irreducible associate classes induces an equivalence of all
associate classes by mapping their unique factor multisets.  If the
equivalence commutes with conjugation, it transports norm classes and their
conjugation orbits.
-/

namespace A374275Lean
namespace GoldenRing

open Associates

abbrev IrredClass := {a : Associates R // Irreducible a}

/-- Conjugation as an involution of irreducible associate classes. -/
def conjIrred : IrredClass ≃ IrredClass where
  toFun p := ⟨conjAssoc p.1, conjAssoc_irreducible p.2⟩
  invFun p := ⟨conjAssoc p.1, conjAssoc_irreducible p.2⟩
  left_inv p := by
    apply Subtype.ext
    exact conjAssoc_involutive p.1
  right_inv p := by
    apply Subtype.ext
    exact conjAssoc_involutive p.1

@[simp] lemma conjIrred_val (p : IrredClass) :
    (conjIrred p).1 = conjAssoc p.1 := rfl

@[simp] lemma conjIrred_involutive (p : IrredClass) :
    conjIrred (conjIrred p) = p := conjIrred.left_inv p

lemma multiset_map_eq_self_of_forall_mem {f : IrredClass → IrredClass}
    {s : Multiset IrredClass} (h : ∀ p ∈ s, f p = p) :
    s.map f = s := by
  induction s using Multiset.induction_on with
  | empty => simp
  | @cons p s ih =>
      rw [Multiset.map_cons, h p (Multiset.mem_cons_self p s), ih]
      intro q hq
      exact h q (Multiset.mem_cons_of_mem hq)

/-- Mapping a multiset by an equivalence, as an additive homomorphism. -/
def multisetMapAddHom (σ : IrredClass ≃ IrredClass) :
    Multiset IrredClass →+ Multiset IrredClass where
  toFun := Multiset.map σ
  map_zero' := Multiset.map_zero _
  map_add' := Multiset.map_add _

@[simp] lemma multisetMapAddHom_apply (σ : IrredClass ≃ IrredClass)
    (s : Multiset IrredClass) :
    multisetMapAddHom σ s = s.map σ := rfl

/-- Map a factor multiset, preserving the top element representing zero. -/
noncomputable def mapFactorSet (σ : IrredClass ≃ IrredClass) :
    Associates.FactorSet R → Associates.FactorSet R :=
  WithTop.map (multisetMapAddHom σ)

@[simp] lemma mapFactorSet_top (σ : IrredClass ≃ IrredClass) :
    mapFactorSet σ (⊤ : Associates.FactorSet R) = ⊤ := by
  simp [mapFactorSet]

@[simp] lemma mapFactorSet_coe (σ : IrredClass ≃ IrredClass)
    (s : Multiset IrredClass) :
    mapFactorSet σ (s : Associates.FactorSet R) = (s.map σ : Multiset IrredClass) := by
  rfl

lemma mapFactorSet_add (σ : IrredClass ≃ IrredClass)
    (s t : Associates.FactorSet R) :
    mapFactorSet σ (s + t) = mapFactorSet σ s + mapFactorSet σ t := by
  exact WithTop.map_add (multisetMapAddHom σ) s t

@[simp] lemma mapFactorSet_symm_mapFactorSet (σ : IrredClass ≃ IrredClass)
    (s : Associates.FactorSet R) :
    mapFactorSet σ.symm (mapFactorSet σ s) = s := by
  induction s with
  | none => rfl
  | some s =>
      change mapFactorSet σ.symm (mapFactorSet σ
        (s : Associates.FactorSet R)) = (s : Associates.FactorSet R)
      rw [mapFactorSet_coe, mapFactorSet_coe, WithTop.coe_eq_coe]
      rw [Multiset.map_map]
      simp

@[simp] lemma mapFactorSet_mapFactorSet_symm (σ : IrredClass ≃ IrredClass)
    (s : Associates.FactorSet R) :
    mapFactorSet σ (mapFactorSet σ.symm s) = s := by
  induction s with
  | none => rfl
  | some s =>
      change mapFactorSet σ (mapFactorSet σ.symm
        (s : Associates.FactorSet R)) = (s : Associates.FactorSet R)
      rw [mapFactorSet_coe, mapFactorSet_coe, WithTop.coe_eq_coe]
      rw [Multiset.map_map]
      simp

/-- The associate class obtained by applying `σ` to every irreducible factor. -/
noncomputable def transportAssoc (σ : IrredClass ≃ IrredClass)
    (a : Associates R) : Associates R :=
  (mapFactorSet σ a.factors).prod

@[simp] lemma factors_transportAssoc (σ : IrredClass ≃ IrredClass)
    (a : Associates R) :
    (transportAssoc σ a).factors = mapFactorSet σ a.factors := by
  exact Associates.prod_factors _

@[simp] lemma transportAssoc_symm_transportAssoc (σ : IrredClass ≃ IrredClass)
    (a : Associates R) :
    transportAssoc σ.symm (transportAssoc σ a) = a := by
  apply Associates.eq_of_factors_eq_factors
  simp

@[simp] lemma transportAssoc_transportAssoc_symm (σ : IrredClass ≃ IrredClass)
    (a : Associates R) :
    transportAssoc σ (transportAssoc σ.symm a) = a := by
  apply Associates.eq_of_factors_eq_factors
  simp

/-- Factor transport as an equivalence of associate classes. -/
noncomputable def transportAssocEquiv (σ : IrredClass ≃ IrredClass) :
    Associates R ≃ Associates R where
  toFun := transportAssoc σ
  invFun := transportAssoc σ.symm
  left_inv := transportAssoc_symm_transportAssoc σ
  right_inv := transportAssoc_transportAssoc_symm σ

lemma transportAssoc_mul (σ : IrredClass ≃ IrredClass) (a b : Associates R) :
    transportAssoc σ (a * b) = transportAssoc σ a * transportAssoc σ b := by
  apply Associates.eq_of_factors_eq_factors
  simp only [factors_transportAssoc, Associates.factors_mul]
  exact mapFactorSet_add σ a.factors b.factors

lemma multiset_prod_map_conjIrred (s : Multiset IrredClass) :
    ((s.map conjIrred).map Subtype.val).prod =
      conjAssoc ((s.map Subtype.val).prod) := by
  induction s using Multiset.induction_on with
  | empty => simp
  | @cons p s ih =>
      simp only [Multiset.map_cons, Multiset.prod_cons, conjIrred_val]
      rw [ih, map_mul]

lemma prod_mapFactorSet_conjIrred (s : Associates.FactorSet R) :
    (mapFactorSet conjIrred s).prod = conjAssoc s.prod := by
  induction s with
  | none =>
      change 0 = conjAssoc 0
      simp
  | some s => exact multiset_prod_map_conjIrred s

lemma factors_conjAssoc (a : Associates R) :
    (conjAssoc a).factors = mapFactorSet conjIrred a.factors := by
  apply Associates.FactorSet.unique
  rw [Associates.factors_prod, prod_mapFactorSet_conjIrred,
    Associates.factors_prod]

/-- A factor permutation respects conjugation when it commutes with the
involution on irreducible associate classes. -/
def CommutesConj (σ : IrredClass ≃ IrredClass) : Prop :=
  ∀ p, σ (conjIrred p) = conjIrred (σ p)

lemma mapFactorSet_conj_comm (σ : IrredClass ≃ IrredClass)
    (hσ : CommutesConj σ) (s : Associates.FactorSet R) :
    mapFactorSet σ (mapFactorSet conjIrred s) =
      mapFactorSet conjIrred (mapFactorSet σ s) := by
  induction s with
  | none => rfl
  | some s =>
      change mapFactorSet σ (mapFactorSet conjIrred
        (s : Associates.FactorSet R)) = mapFactorSet conjIrred
          (mapFactorSet σ (s : Associates.FactorSet R))
      rw [mapFactorSet_coe, mapFactorSet_coe, mapFactorSet_coe,
        mapFactorSet_coe, WithTop.coe_eq_coe]
      rw [Multiset.map_map, Multiset.map_map]
      apply Multiset.map_congr rfl
      intro p hp
      exact hσ p

lemma transportAssoc_conj_comm (σ : IrredClass ≃ IrredClass)
    (hσ : CommutesConj σ) (a : Associates R) :
    transportAssoc σ (conjAssoc a) = conjAssoc (transportAssoc σ a) := by
  apply Associates.eq_of_factors_eq_factors
  rw [factors_transportAssoc, factors_conjAssoc, factors_conjAssoc,
    factors_transportAssoc]
  exact mapFactorSet_conj_comm σ hσ a.factors

lemma transportAssoc_zero (σ : IrredClass ≃ IrredClass) :
    transportAssoc σ 0 = 0 := by
  apply Associates.eq_of_factors_eq_factors
  simp

/-- A conjugation-compatible factor permutation carrying the integer class
`k` to `k'` carries all norm classes of `k` to norm classes of `k'`. -/
lemma transportAssoc_mem_normClasses {σ : IrredClass ≃ IrredClass}
    (hσ : CommutesConj σ) {k k' : ℕ} (hk : 0 < k) (hk' : 0 < k')
    (hkk' : transportAssoc σ (Associates.mk (k : R)) =
      Associates.mk (k' : R)) {a : Associates R} (ha : a ∈ normClasses k) :
    transportAssoc σ a ∈ normClasses k' := by
  apply (mem_normClasses_iff_mul_conj hk').mpr
  calc
    transportAssoc σ a * conjAssoc (transportAssoc σ a) =
        transportAssoc σ a * transportAssoc σ (conjAssoc a) := by
          rw [transportAssoc_conj_comm σ hσ]
    _ = transportAssoc σ (a * conjAssoc a) :=
      (transportAssoc_mul σ a (conjAssoc a)).symm
    _ = transportAssoc σ (Associates.mk (k : R)) := by
      rw [(mem_normClasses_iff_mul_conj hk).mp ha]
    _ = Associates.mk (k' : R) := hkk'

/-- Equivalence of norm classes induced by a conjugation-compatible factor
permutation carrying `k` to `k'`. -/
noncomputable def normClassTransportEquiv {σ : IrredClass ≃ IrredClass}
    (hσ : CommutesConj σ) {k k' : ℕ} (hk : 0 < k) (hk' : 0 < k')
    (hkk' : transportAssoc σ (Associates.mk (k : R)) =
      Associates.mk (k' : R)) : NormClass k ≃ NormClass k' where
  toFun a := ⟨transportAssoc σ a.1,
    transportAssoc_mem_normClasses hσ hk hk' hkk' a.2⟩
  invFun b := ⟨transportAssoc σ.symm b.1, by
    have hback : transportAssoc σ.symm (Associates.mk (k' : R)) =
        Associates.mk (k : R) := by
      have := congrArg (transportAssoc σ.symm) hkk'
      simpa using this.symm
    have hσsymm : CommutesConj σ.symm := by
      intro p
      apply σ.injective
      rw [σ.apply_symm_apply, hσ, σ.apply_symm_apply]
    exact transportAssoc_mem_normClasses hσsymm hk' hk hback b.2⟩
  left_inv a := by
    apply Subtype.ext
    exact transportAssoc_symm_transportAssoc σ a.1
  right_inv b := by
    apply Subtype.ext
    exact transportAssoc_transportAssoc_symm σ b.1

lemma normClassTransportEquiv_conj {σ : IrredClass ≃ IrredClass}
    (hσ : CommutesConj σ) {k k' : ℕ} (hk : 0 < k) (hk' : 0 < k')
    (hkk' : transportAssoc σ (Associates.mk (k : R)) =
      Associates.mk (k' : R)) (a : NormClass k) :
    normClassTransportEquiv hσ hk hk' hkk' (conjNormClass hk a) =
      conjNormClass hk' (normClassTransportEquiv hσ hk hk' hkk' a) := by
  apply Subtype.ext
  exact transportAssoc_conj_comm σ hσ a.1

/-- The corresponding conjugation-orbit equivalence. -/
noncomputable def normClassOrbitTransportEquiv {σ : IrredClass ≃ IrredClass}
    (hσ : CommutesConj σ) {k k' : ℕ} (hk : 0 < k) (hk' : 0 < k')
    (hkk' : transportAssoc σ (Associates.mk (k : R)) =
      Associates.mk (k' : R)) : NormClassOrbit hk ≃ NormClassOrbit hk' :=
  Quotient.congr (normClassTransportEquiv hσ hk hk' hkk') fun a b ↦ by
    change (b = a ∨ b = conjNormClass hk a) ↔
      (normClassTransportEquiv hσ hk hk' hkk' b =
        normClassTransportEquiv hσ hk hk' hkk' a ∨
       normClassTransportEquiv hσ hk hk' hkk' b =
        conjNormClass hk' (normClassTransportEquiv hσ hk hk' hkk' a))
    constructor
    · rintro (rfl | rfl)
      · exact Or.inl rfl
      · exact Or.inr (normClassTransportEquiv_conj hσ hk hk' hkk' a)
    · rintro (hsame | hconj)
      · exact Or.inl ((normClassTransportEquiv hσ hk hk' hkk').injective hsame)
      · right
        apply (normClassTransportEquiv hσ hk hk' hkk').injective
        rw [normClassTransportEquiv_conj hσ hk hk' hkk']
        exact hconj

theorem representationCount_eq_of_factor_transport {σ : IrredClass ≃ IrredClass}
    (hσ : CommutesConj σ) {k k' : ℕ} (hk : 0 < k) (hk' : 0 < k')
    (hkk' : transportAssoc σ (Associates.mk (k : R)) =
      Associates.mk (k' : R)) :
    representationCount k = representationCount k' := by
  rw [representationCount_eq_natCard_orbits hk,
    representationCount_eq_natCard_orbits hk']
  exact Nat.card_congr (normClassOrbitTransportEquiv hσ hk hk' hkk')

end GoldenRing
end A374275Lean
