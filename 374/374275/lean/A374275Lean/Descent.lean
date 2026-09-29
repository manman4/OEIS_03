import A374275Lean.Definitions

/-!
# The minimality descent for A374275

The algebraic-number-theory argument supplies the replacement property below:
if a represented value with at least two representations is not divisible by
11, replacing one split prime by the smaller split prime 11 produces a
strictly smaller value with the same representation count.

This file proves that this replacement property forces every positive-index
term of A374275 to be divisible by 11.
-/

namespace A374275Lean

/--
The exact descent property supplied by the prime-replacement argument in
Z[(1+sqrt(5))/2].
-/
def ElevenReplacement : Prop :=
  ∀ k, 2 ≤ representationCount k → ¬ 11 ∣ k →
    ∃ k', k' < k ∧ representationCount k' = representationCount k

/--
For the exact OEIS definition, the prime-replacement descent implies
11 ∣ a374275 n for every positive n.

Existence is explicit because a374275 has a default value when its defining
set is empty.
-/
theorem eleven_dvd_a374275_of_replacement
    (hreplace : ElevenReplacement) {n : ℕ} (hn : 0 < n)
    (hexists : ∃ k, representationCount k = n) :
    11 ∣ a374275 n := by
  by_cases hn1 : n = 1
  · subst n
    simp [a374275_one]
  · have hn2 : 2 ≤ n := by omega
    by_contra hnot
    have hcount : representationCount (a374275 n) = n :=
      a374275_spec hexists
    have hcount2 : 2 ≤ representationCount (a374275 n) := by
      omega
    obtain ⟨k', hk'lt, hk'count⟩ :=
      hreplace (a374275 n) hcount2 hnot
    have hk'min : a374275 n ≤ k' := by
      apply a374275_min hexists
      rw [hk'count, hcount]
    omega

end A374275Lean
