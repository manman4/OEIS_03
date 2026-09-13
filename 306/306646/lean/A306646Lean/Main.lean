import A306646Lean.PrimeDivisibility

/-!
# A306646: main theorem

The public theorem is `A306646Lean.prime_dvd_a306646`:

```lean
theorem prime_dvd_a306646 {p k : ℕ}
    (hp : Nat.Prime p) (hk : 2 ≤ k) :
    p ∣ a306646 p k
```
-/
