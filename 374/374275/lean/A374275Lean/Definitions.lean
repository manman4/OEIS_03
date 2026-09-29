import Mathlib.Data.Finset.Prod
import Mathlib.Data.Nat.Find
import Mathlib.Tactic

/-!
# Exact finite definitions for A374275

The quadratic form is x^2 + 3*x*y + y^2. For a fixed k, every
nonnegative solution has x ≤ k and y ≤ k, so searching the square
{0, ..., k} × {0, ..., k} is exact rather than a cutoff.

a374275 n is the least k with exactly n solutions satisfying
0 ≤ x ≤ y, when such a k exists, and is 0 otherwise.
-/

namespace A374275Lean

/-- The quadratic form defining A374275. -/
def form (x y : ℕ) : ℕ :=
  x * x + 3 * x * y + y * y

/-- All A374275 solutions at level k, in an exact finite bounding square. -/
def solutions (k : ℕ) : Finset (ℕ × ℕ) :=
  ((Finset.range (k + 1)).product (Finset.range (k + 1))).filter
    fun pair => pair.1 ≤ pair.2 ∧ form pair.1 pair.2 = k

/-- Number of nonnegative solutions with x ≤ y. -/
def representationCount (k : ℕ) : ℕ :=
  (solutions k).card

/--
The sequence A374275, with a harmless default value 0 if the requested
representation count does not occur.
-/
noncomputable def a374275 (n : ℕ) : ℕ :=
  by
    classical
    exact if h : ∃ k, representationCount k = n then Nat.find h else 0

theorem mem_solutions_iff {k x y : ℕ} :
    (x, y) ∈ solutions k ↔ x ≤ y ∧ form x y = k := by
  constructor
  · intro h
    exact (Finset.mem_filter.mp h).2
  · rintro ⟨hxy, hform⟩
    apply Finset.mem_filter.mpr
    constructor
    · apply Finset.mem_product.mpr
      constructor <;> apply Finset.mem_range.mpr
      · have hxx : x ≤ x * x := by nlinarith
        have hxform : x ≤ form x y := by
          calc
            x ≤ x * x := hxx
            _ ≤ form x y := by
              rw [form, Nat.add_assoc]
              exact Nat.le_add_right _ _
        have hxk : x ≤ k := hform ▸ hxform
        omega
      · have hyy : y ≤ y * y := by nlinarith
        have hyform : y ≤ form x y := by
          calc
            y ≤ y * y := hyy
            _ ≤ form x y := by
              rw [form]
              exact Nat.le_add_left _ _
        have hyk : y ≤ k := hform ▸ hyform
        omega
    · exact ⟨hxy, hform⟩

theorem representationCount_zero : representationCount 0 = 1 := by
  rfl

theorem exists_count_one : ∃ k, representationCount k = 1 :=
  ⟨0, representationCount_zero⟩

theorem a374275_spec {n : ℕ} (hexists : ∃ k, representationCount k = n) :
    representationCount (a374275 n) = n := by
  rw [a374275, dif_pos hexists]
  exact Nat.find_spec hexists

theorem a374275_min {n k : ℕ} (hexists : ∃ j, representationCount j = n)
    (hk : representationCount k = n) :
    a374275 n ≤ k := by
  rw [a374275, dif_pos hexists]
  exact Nat.find_min' hexists hk

theorem a374275_one : a374275 1 = 0 := by
  apply Nat.eq_zero_of_le_zero
  exact a374275_min exists_count_one representationCount_zero

end A374275Lean
