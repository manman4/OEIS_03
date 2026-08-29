import A330432Lean.Definitions

/-!
# The A330432 product formula
-/

namespace A330432Lean

open Finset

theorem allowedSet_mono {n k l : ℕ} (hkl : k ≤ l) :
    allowedSet n k ⊆ allowedSet n l := by
  intro x hx
  rcases Finset.mem_filter.mp hx with ⟨hrange, hineq⟩
  apply Finset.mem_filter.mpr
  exact ⟨hrange, hineq.trans (Nat.mul_le_mul_right x hkl)⟩

theorem allowedSets_pairwise (n : ℕ) :
    (allowedSets n).Pairwise (· ⊆ ·) := by
  rw [allowedSets, List.pairwise_map]
  exact (List.pairwise_lt_range' (s := 1) (n := n)).imp fun h =>
    allowedSet_mono (Nat.le_of_lt h)

theorem allowedSet_eq_Icc {n k : ℕ} (hk : 0 < k) :
    allowedSet n k = Finset.Icc (n ⌈/⌉ k) n := by
  ext x
  simp [allowedSet, Nat.lt_succ_iff, ceilDiv_le_iff_le_mul hk, and_comm]

theorem allowedSet_card {n k : ℕ} (hk : 0 < k) :
    (allowedSet n k).card = n + 1 - (n ⌈/⌉ k) := by
  rw [allowedSet_eq_Icc hk]
  exact Nat.card_Icc _ _

theorem allowedSet_card_sub {n k : ℕ} (hk : 0 < k) (hkn : k ≤ n) :
    (allowedSet n k).card - (k - 1) =
      n - k + 2 - (n ⌈/⌉ k) := by
  rw [allowedSet_card hk]
  have hmul : n ≤ k * n := by
    simpa using Nat.mul_le_mul_right n hk
  have hceil : n ⌈/⌉ k ≤ n :=
    (ceilDiv_le_iff_le_mul hk).2 hmul
  omega

theorem choiceProduct_allowed_range (n start len used : ℕ)
    (hstart : start = used + 1) :
    choiceProduct used
        ((List.range' start len).map (allowedSet n)) =
      ((List.range' start len).map fun k =>
        (allowedSet n k).card - (k - 1)).prod := by
  induction len generalizing start used with
  | zero =>
      simp [choiceProduct]
  | succ len ih =>
      rw [List.range'_succ]
      simp only [List.map_cons, choiceProduct, List.prod_cons]
      have hnext : start + 1 = (used + 1) + 1 := by omega
      rw [ih (start + 1) (used + 1) hnext]
      congr 1
      omega

theorem card_a330432Choice_eq_product (n : ℕ) :
    Fintype.card (A330432Choice n) = a330432 n := by
  rw [show Fintype.card (A330432Choice n) =
      choiceProduct 0 (allowedSets n) by
    exact card_choices_eq_choiceProduct ∅ (allowedSets n)
      (allowedSets_pairwise n) (by simp)]
  rw [allowedSets, oneTo,
    choiceProduct_allowed_range n 1 n 0 (by omega)]
  have hmap :
      (List.range' 1 n).map
          (fun k => (allowedSet n k).card - (k - 1)) =
        (List.range' 1 n).map
          (fun k => n - k + 2 - (n ⌈/⌉ k)) := by
    apply List.map_congr_left
    intro k hk
    have hbounds : 1 ≤ k ∧ k ≤ n := by
      rw [List.mem_range'_1] at hk
      omega
    exact allowedSet_card_sub hbounds.1 hbounds.2
  rw [hmap]
  have hfinset :
      (List.range' 1 n).toFinset = Finset.Icc 1 n := by
    ext k
    simp only [List.mem_toFinset, List.mem_range'_1, Finset.mem_Icc]
    omega
  rw [← List.prod_toFinset
    (fun k => n - k + 2 - (n ⌈/⌉ k)) List.nodup_range']
  rw [hfinset]
  rfl

end A330432Lean
