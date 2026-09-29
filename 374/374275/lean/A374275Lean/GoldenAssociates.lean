import A374275Lean.GoldenReduction
import Mathlib.RingTheory.UniqueFactorizationDomain.FactorSet

/-!
# Conjugation on associate classes

The nontrivial Galois automorphism descends to the associate monoid.  Its
orbits encode exchanging the two variables of the quadratic form.
-/

namespace A374275Lean
namespace GoldenRing

open QuadraticAlgebra

/-- Conjugation on associate classes. -/
def conjAssoc : Associates R →* Associates R where
  toFun a := Quotient.liftOn a (fun z : R ↦ Associates.mk (star z)) (by
    intro z w h
    exact Quotient.sound (Associated.map (starRingEnd R) h))
  map_one' := by
    change Associates.mk (star (1 : R)) = 1
    simp
  map_mul' a b := by
    induction a using Quotient.inductionOn with | _ a => ?_
    induction b using Quotient.inductionOn with | _ b => ?_
    change Associates.mk (star (a * b)) =
      Associates.mk (star a) * Associates.mk (star b)
    simp [Associates.mk_mul_mk]

@[simp] lemma conjAssoc_mk (z : R) :
    conjAssoc (Associates.mk z) = Associates.mk (star z) := rfl

@[simp] lemma conjAssoc_involutive (a : Associates R) :
    conjAssoc (conjAssoc a) = a := by
  induction a using Quotient.inductionOn with | _ z => ?_
  change Associates.mk (star (star z)) = Associates.mk z
  simp

@[simp] lemma conjAssoc_zero : conjAssoc (0 : Associates R) = 0 := by
  change Associates.mk (star (0 : R)) = Associates.mk (0 : R)
  simp

lemma conjAssoc_injective : Function.Injective conjAssoc :=
  Function.LeftInverse.injective conjAssoc_involutive

@[simp] lemma conjAssoc_eq_iff {a b : Associates R} :
    conjAssoc a = conjAssoc b ↔ a = b := conjAssoc_injective.eq_iff

@[simp] lemma mk_mul_conjAssoc_mk (z : R) :
    Associates.mk z * conjAssoc (Associates.mk z) =
      Associates.mk ((QuadraticAlgebra.norm z : ℤ) : R) := by
  rw [conjAssoc_mk, Associates.mk_mul_mk]
  apply Associates.mk_eq_mk_iff_associated.mpr
  apply Associated.of_eq
  exact (QuadraticAlgebra.algebraMap_norm_eq_mul_star z).symm

@[simp] lemma conjAssoc_natCast (k : ℕ) :
    conjAssoc (Associates.mk (k : R)) = Associates.mk (k : R) := by
  change Associates.mk (star (k : R)) = Associates.mk (k : R)
  simp

@[simp] lemma conjAssoc_intCast (k : ℤ) :
    conjAssoc (Associates.mk (k : R)) = Associates.mk (k : R) := by
  change Associates.mk (star (k : R)) = Associates.mk (k : R)
  simp

lemma conjAssoc_irreducible {p : Associates R} (hp : Irreducible p) :
    Irreducible (conjAssoc p) := by
  rw [irreducible_iff] at hp ⊢
  constructor
  · intro hu
    apply hp.1
    rcases hu with ⟨u, hu⟩
    refine ⟨Units.map conjAssoc u, ?_⟩
    simpa using congrArg conjAssoc hu
  · intro a b hab
    have hab' : p = conjAssoc a * conjAssoc b := by
      have := congrArg conjAssoc hab
      simpa using this
    rcases hp.2 hab' with ha | hb
    · left
      rcases ha with ⟨u, hu⟩
      refine ⟨Units.map conjAssoc u, ?_⟩
      simpa using congrArg conjAssoc hu
    · right
      rcases hb with ⟨u, hu⟩
      refine ⟨Units.map conjAssoc u, ?_⟩
      simpa using congrArg conjAssoc hu

end GoldenRing
end A374275Lean
