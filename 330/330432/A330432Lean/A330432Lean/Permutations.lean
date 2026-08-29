import A330432Lean.ChoiceEncoding

/-!
# Ordinary A330432 permutations and the nested-choice encoding

The main result of this file is a bijection between the recursive choices
counted by the product formula and ordinary permutations of `[1, ..., n]`
satisfying `n <= k * sigma(k)` at every position.
-/

namespace A330432Lean

open Finset

@[simp] theorem length_oneTo (n : ℕ) : (oneTo n).length = n := by
  simp [oneTo]

theorem mem_oneTo {n x : ℕ} : x ∈ oneTo n ↔ 1 ≤ x ∧ x ≤ n := by
  rw [oneTo, List.mem_range'_1]
  omega

theorem nodup_oneTo (n : ℕ) : (oneTo n).Nodup := by
  simpa [oneTo] using List.nodup_range' (s := 1) (n := n)

/-- Strengthen a pointwise relation using the fact that both related entries
belong to the two lists. -/
theorem forall₂_imp_of_mem {α β : Type*} {R S : α → β → Prop}
    {xs : List α} {ys : List β} (h : List.Forall₂ R xs ys)
    (H : ∀ a ∈ xs, ∀ b ∈ ys, R a b → S a b) :
    List.Forall₂ S xs ys := by
  induction h with
  | nil => exact List.Forall₂.nil
  | @cons a b xs ys hab htail ih =>
      apply List.Forall₂.cons
      · exact H a (by simp) b (by simp) hab
      · apply ih
        intro x hx y hy hxy
        exact H x (by simp [hx]) y (by simp [hy]) hxy

theorem A330432Permutation.values_mem_permutations {n : ℕ}
    (p : A330432Permutation n) :
    p.values ∈ (oneTo n).permutations := by
  exact List.get_mem _ p.1

theorem A330432Permutation.values_perm {n : ℕ}
    (p : A330432Permutation n) : p.values.Perm (oneTo n) :=
  List.perm_of_mem_permutations p.values_mem_permutations

@[simp] theorem A330432Permutation.length_values {n : ℕ}
    (p : A330432Permutation n) : p.values.length = n := by
  rw [p.values_perm.length_eq, length_oneTo]

theorem A330432Permutation.mem_values_iff {n x : ℕ}
    (p : A330432Permutation n) : x ∈ p.values ↔ 1 ≤ x ∧ x ≤ n := by
  rw [p.values_perm.mem_iff, mem_oneTo]

theorem A330432Permutation.nodup_values {n : ℕ}
    (p : A330432Permutation n) : p.values.Nodup :=
  p.values_perm.nodup_iff.mpr (nodup_oneTo n)

theorem A330432Permutation.satisfies {n : ℕ}
    (p : A330432Permutation n) : SatisfiesA330432 n p.values :=
  p.2

theorem A330432Permutation.ext_values {n : ℕ}
    {p q : A330432Permutation n} (h : p.values = q.values) : p = q := by
  apply Subtype.ext
  apply (List.nodup_permutations (oneTo n) (nodup_oneTo n)).get_inj_iff.mp
  exact h

/-- Package an ordinary valid permutation list in the finite enumeration. -/
noncomputable def A330432Permutation.ofList {n : ℕ} (xs : List ℕ)
    (hperm : xs.Perm (oneTo n))
    (hsat : SatisfiesA330432 n xs) : A330432Permutation n := by
  classical
  let all := (oneTo n).permutations
  have hmem : xs ∈ all := List.mem_permutations.mpr hperm
  have hidx : all.idxOf xs < all.length := List.idxOf_lt_length_iff.mpr hmem
  let i : Fin all.length := ⟨all.idxOf xs, hidx⟩
  refine ⟨i, ?_⟩
  have hget : all.get i = xs := List.getElem_idxOf hidx
  change SatisfiesA330432 n (all.get i)
  rwa [hget]

@[simp] theorem A330432Permutation.values_ofList {n : ℕ} (xs : List ℕ)
    (hperm : xs.Perm (oneTo n))
    (hsat : SatisfiesA330432 n xs) :
    (A330432Permutation.ofList xs hperm hsat).values = xs := by
  classical
  unfold A330432Permutation.ofList A330432Permutation.values
  simp only
  apply List.getElem_idxOf

/-- A recursive choice uses every number in `[1, ..., n]` exactly once. -/
theorem A330432Choice.toList_perm {n : ℕ} (c : A330432Choice n) :
    c.toList.Perm (oneTo n) := by
  have hrel :
      List.Forall₂ (fun x k => x ∈ allowedSet n k)
        c.toList (oneTo n) := by
    simpa [A330432Choice, allowedSets] using c.forall₂_toList
  have hstrong :
      List.Forall₂ (fun x k => 1 ≤ x ∧ x ≤ n ∧ n ≤ k * x)
        c.toList (oneTo n) :=
    forall₂_imp_of_mem hrel (by
      intro x _hx k hk hxallowed
      have hkbound := mem_oneTo.mp hk
      rcases Finset.mem_filter.mp hxallowed with ⟨hxrange, hineq⟩
      have hxupper : x ≤ n := by
        simpa only [Finset.mem_range, Nat.lt_add_one_iff] using hxrange
      have hxpositive : 1 ≤ x := by
        cases x with
        | zero =>
            simp only [Nat.mul_zero] at hineq
            omega
        | succ x => omega
      exact ⟨hxpositive, hxupper, hineq⟩)
  have hsubset : c.toList.toFinset ⊆ (oneTo n).toFinset := by
    intro x hx
    have hxlist : x ∈ c.toList := by simpa using hx
    have hlower : ∀ y ∈ c.toList, 1 ≤ y :=
      (List.forall₂_and_left c.toList (oneTo n)).mp hstrong |>.1
    have hrest :
        List.Forall₂ (fun y k => y ≤ n ∧ n ≤ k * y)
          c.toList (oneTo n) :=
      (List.forall₂_and_left c.toList (oneTo n)).mp hstrong |>.2
    have hupper : ∀ y ∈ c.toList, y ≤ n :=
      (List.forall₂_and_left c.toList (oneTo n)).mp hrest |>.1
    have : x ∈ oneTo n :=
      mem_oneTo.mpr ⟨hlower x hxlist, hupper x hxlist⟩
    simpa using this
  have hcard : (oneTo n).toFinset.card ≤ c.toList.toFinset.card := by
    rw [List.toFinset_card_of_nodup (nodup_oneTo n),
      List.toFinset_card_of_nodup c.nodup_toList,
      length_oneTo, c.length_toList]
    simp [allowedSets, oneTo]
  have heq : c.toList.toFinset = (oneTo n).toFinset :=
    Finset.eq_of_subset_of_card_le hsubset hcard
  apply (List.perm_ext_iff_of_nodup c.nodup_toList (nodup_oneTo n)).mpr
  intro x
  simpa only [List.mem_toFinset] using Finset.ext_iff.mp heq x

/-- A recursive choice satisfies the defining positionwise inequalities. -/
theorem A330432Choice.toList_satisfies {n : ℕ}
    (c : A330432Choice n) : SatisfiesA330432 n c.toList := by
  have hrel :
      List.Forall₂ (fun x k => x ∈ allowedSet n k)
        c.toList (oneTo n) := by
    simpa [A330432Choice, allowedSets] using c.forall₂_toList
  exact hrel.imp fun x k hx => (Finset.mem_filter.mp hx).2

/-- Turn a recursive choice into the ordinary permutation it represents. -/
noncomputable def A330432Choice.toPermutation {n : ℕ}
    (c : A330432Choice n) : A330432Permutation n :=
  A330432Permutation.ofList c.toList c.toList_perm c.toList_satisfies

@[simp] theorem A330432Choice.values_toPermutation {n : ℕ}
    (c : A330432Choice n) : c.toPermutation.values = c.toList := by
  simp [A330432Choice.toPermutation]

/-- Every ordinary valid permutation has a recursive-choice encoding. -/
theorem A330432Permutation.exists_choice {n : ℕ}
    (p : A330432Permutation n) :
    ∃ c : A330432Choice n, c.toList = p.values := by
  have hrel :
      List.Forall₂ (fun x k => x ∈ allowedSet n k)
        p.values (oneTo n) :=
    forall₂_imp_of_mem p.satisfies (by
      intro x hx _k _hk hineq
      apply Finset.mem_filter.mpr
      exact ⟨Finset.mem_range.mpr (by
        have := (p.mem_values_iff.mp hx).2
        omega), hineq⟩)
  have hchoice : IsChoiceList ∅ (allowedSets n) p.values := by
    refine ⟨?_, p.nodup_values, by simp, ?_⟩
    · simp [allowedSets, oneTo]
    · simpa [allowedSets] using hrel
  exact exists_choices_of_isChoiceList hchoice

theorem A330432Choice.toPermutation_injective {n : ℕ} :
    Function.Injective
      (A330432Choice.toPermutation :
        A330432Choice n → A330432Permutation n) := by
  intro c d h
  apply Choices.toList_injective ∅ (allowedSets n)
  have := congrArg A330432Permutation.values h
  simpa using this

theorem A330432Choice.toPermutation_surjective {n : ℕ} :
    Function.Surjective
      (A330432Choice.toPermutation :
        A330432Choice n → A330432Permutation n) := by
  intro p
  obtain ⟨c, hc⟩ := p.exists_choice
  refine ⟨c, A330432Permutation.ext_values ?_⟩
  simpa using hc

/-- The exact bijection between the counted permutations and nested choices. -/
noncomputable def choiceEquivA330432Permutation (n : ℕ) :
    A330432Choice n ≃ A330432Permutation n :=
  Equiv.ofBijective A330432Choice.toPermutation
    ⟨A330432Choice.toPermutation_injective,
      A330432Choice.toPermutation_surjective⟩

end A330432Lean
