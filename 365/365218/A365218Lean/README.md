# A365218: positivity of every coefficient in Lean 4

[日本語版 / Japanese version](README_JA.md)

This project formalizes the positivity conjecture for
[OEIS A365218](https://oeis.org/A365218). Its generating function satisfies

```math
A(x)=1+\frac{xA(x)^6}{1+xA(x)^6}.
```

The main end-to-end theorem states that every coefficient of every formal
power-series solution of this equation is positive:

```lean
theorem generatingFunction_coeff_pos {A : ℚ⟦X⟧}
    (hA : IsA365218GeneratingFunction A) (n : ℕ) :
    0 < PowerSeries.coeff n A
```

The formal equation uses multiplication by the power-series inverse, which is
the algebraic meaning of the displayed fraction:

```lean
def IsA365218GeneratingFunction (A : ℚ⟦X⟧) : Prop :=
  A = 1 + (PowerSeries.X * A ^ 6) *
    (1 + PowerSeries.X * A ^ 6)⁻¹
```

## Proof outline

Set $`B=A-1`$. The original equation is first converted, entirely in Lean, to

```math
B=x(1-B)(1+B)^6.
```

The project then proves the required one-variable Lagrange inversion theorem
for formal power series over $`\mathbb{Q}`$. This proof is included in the
repository; it is not assumed as an axiom or imported as an unavailable
Mathlib theorem. The proof uses formal Laurent-series residues and establishes

```math
[x^n]B(x)=\frac1n[t^{n-1}]\bigl((1-t)(1+t)^6\bigr)^n
```

for $`n\geq1`$. Consequently,

```math
a(n)=\frac1n[t^{n-1}](1-t)^n(1+t)^{6n}.
```

For fixed $`n`$, write

```math
C_n(t)=(1-t)^n(1+t)^{6n}=\sum_{k\geq0}c_k t^k.
```

Formal differentiation and coefficient comparison give

```math
(1-t^2)C_n'(t)=n(5-7t)C_n(t)
```

and

```math
(k+1)c_{k+1}=5nc_k-(7n-k+1)c_{k-1}.
```

Lean then proves simultaneously, by induction on $`k`$, that

```math
c_k\gt0,
\qquad
2n c_{k-1}\lt k c_k
\qquad(1\leq k\leq n-1).
```

The second inequality is the denominator-free form of

```math
\frac{c_k}{c_{k-1}}\gt\frac{2n}{k}.
```

Taking $`k=n-1`$ proves $`c_{n-1}\gt0`$, hence $`a(n)\gt0`$.

## Main formal results

- `lagrange_inversion_coeff`: the formal Lagrange inversion theorem used here.
- `generatingFunction_sub_one_fixedPoint`: conversion of the OEIS equation to
  $`B=x(1-B)(1+B)^6`$.
- `generatingFunction_coeff_eq_a365218`: every solution has the coefficient
  formula defining `a365218`.
- `a365218_pos`: the coefficient formula is positive for every index.
- `generatingFunction_coeff_pos`: the original equation implies positivity of
  every coefficient.
- `generatingFunction_unique`: the original equation has at most one formal
  power-series solution.

The existence of a solution is not constructed as a separate theorem; the
end-to-end positivity theorem is stated for every series satisfying the OEIS
equation. No analytic convergence assumption is used.

## Files

- `A365218Lean/Definitions.lean` — the kernel polynomial and coefficient formula.
- `A365218Lean/LagrangeInversion.lean` — formal Laurent-residue proof of
  Lagrange inversion.
- `A365218Lean/GeneratingFunction.lean` — the original equation, its Lagrange
  form, and the coefficient bridge.
- `A365218Lean/CoefficientRecurrence.lean` — differential identity and
  coefficient recurrence.
- `A365218Lean/Positivity.lean` — ratio induction and positivity.
- `A365218Lean/Main.lean` — public end-to-end theorems.
- `A365218Lean/AxiomAudit.lean` — axiom audit commands.

## Reproducing the verification

Lean 4.27.0 and Mathlib 4.27.0 are pinned by `lean-toolchain` and
`lake-manifest.json`. From the project root, run:

```sh
lake update
lake exe cache get
lake build
lake env lean A365218Lean/AxiomAudit.lean
```

The proof contains no `sorry`, `admit`, `sorryAx`, or project-specific axiom.
The standard axioms reported through Mathlib may include `propext`,
`Classical.choice`, and `Quot.sound`.

## Relocating the project

All project references are relative. The entire directory containing
`lakefile.toml` can be renamed or moved, after which the same commands can be
run from its new location.
