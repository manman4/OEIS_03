import A330432Lean.NestedChoices
import Mathlib.Algebra.Order.Floor.Div
import Mathlib.Order.Interval.Finset.Nat
import Mathlib.Data.List.Range
import Mathlib.Data.List.Permutation

/-!
# Definitions for A330432
-/

namespace A330432Lean

open Finset

/-- The list of natural numbers from 1 through n. -/
def oneTo (n : ℕ) : List ℕ := List.range' 1 n

/-- Values allowed at position k by the inequality n ≤ k*x. -/
def allowedSet (n k : ℕ) : Finset ℕ :=
  (Finset.range (n + 1)).filter fun x => n ≤ k * x

/-- The nested list of allowed sets for positions 1 through n. -/
def allowedSets (n : ℕ) : List (Finset ℕ) :=
  (oneTo n).map (allowedSet n)

/-- A recursive presentation used to count the valid permutations. -/
def A330432Choice (n : ℕ) : Type :=
  Choices ∅ (allowedSets n)

noncomputable instance (n : ℕ) : Fintype (A330432Choice n) :=
  inferInstanceAs (Fintype (Choices ∅ (allowedSets n)))

/-- The positionwise inequalities in the definition of A330432.  The first
list contains the values and the second contains the positions `1, ..., n`. -/
def SatisfiesA330432 (n : ℕ) (xs : List ℕ) : Prop :=
  List.Forall₂ (fun x k => n ≤ k * x) xs (oneTo n)

/-- The finite type of permutations counted by A330432.  An element is an
index into the duplicate-free list of all permutations of `[1, ..., n]`. -/
def A330432Permutation (n : ℕ) :=
  {i : Fin (oneTo n).permutations.length //
    SatisfiesA330432 n ((oneTo n).permutations.get i)}

noncomputable instance (n : ℕ) : Fintype (A330432Permutation n) := by
  classical exact Subtype.fintype _

/-- The ordinary list represented by an A330432 permutation. -/
def A330432Permutation.values {n : ℕ}
    (p : A330432Permutation n) : List ℕ :=
  (oneTo n).permutations.get p.1

/-- The explicit product appearing in the A330432 formula. -/
def a330432 (n : ℕ) : ℕ :=
  ∏ k ∈ Finset.Icc 1 n, (n - k + 2 - (n ⌈/⌉ k))

end A330432Lean
