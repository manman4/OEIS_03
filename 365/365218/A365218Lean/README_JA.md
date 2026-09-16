# A365218 の全係数の正値性 — Lean 4 形式化

[English README](README.md)

このプロジェクトは [OEIS A365218](https://oeis.org/A365218) の正値性予想を
Lean 4 と Mathlib で形式化したものです。母関数は

```math
A(x)=1+\frac{xA(x)^6}{1+xA(x)^6}
```

を満たします。最終的な定理は、この方程式を満たす任意の形式的冪級数の
全係数が正であることを述べます。

```lean
theorem generatingFunction_coeff_pos {A : ℚ⟦X⟧}
    (hA : IsA365218GeneratingFunction A) (n : ℕ) :
    0 < PowerSeries.coeff n A
```

Lean 上では分数を形式的冪級数の逆元との積として表しています。

```lean
def IsA365218GeneratingFunction (A : ℚ⟦X⟧) : Prop :=
  A = 1 + (PowerSeries.X * A ^ 6) *
    (1 + PowerSeries.X * A ^ 6)⁻¹
```

## 証明の概要

$`B=A-1`$ と置き、元の母関数方程式を Lean 内で整理して

```math
B=x(1-B)(1+B)^6
```

を得ます。

次に、今回必要な一変数の Lagrange 反転公式を、$`\mathbb{Q}`$ 上の形式的
冪級数についてプロジェクト内で証明します。Mathlib に存在しない定理を公理として
仮定しているわけではありません。形式的 Laurent 級数の留数を用いて、$`n\geq1`$
に対し

```math
[x^n]B(x)=\frac1n[t^{n-1}]\bigl((1-t)(1+t)^6\bigr)^n
```

を導きます。従って、

```math
a(n)=\frac1n[t^{n-1}](1-t)^n(1+t)^{6n}
```

です。

$`n`$ を固定して

```math
C_n(t)=(1-t)^n(1+t)^{6n}=\sum_{k\geq0}c_k t^k
```

と置きます。形式的に微分して係数を比較すると、

```math
(1-t^2)C_n'(t)=n(5-7t)C_n(t)
```

および

```math
(k+1)c_{k+1}=5nc_k-(7n-k+1)c_{k-1}
```

を得ます。

さらに Lean で $`k`$ に関する帰納法を行い、

```math
c_k\gt0,
\qquad
2n c_{k-1}\lt k c_k
\qquad(1\leq k\leq n-1)
```

を同時に証明します。後者は、分母を払った形で

```math
\frac{c_k}{c_{k-1}}\gt\frac{2n}{k}
```

に対応します。$`k=n-1`$ とすれば $`c_{n-1}\gt0`$、従って
$`a(n)\gt0`$ が従います。

## 主な定理

- `lagrange_inversion_coeff` — 今回使用する Lagrange 反転公式。
- `generatingFunction_sub_one_fixedPoint` — 元の方程式から
  $`B=x(1-B)(1+B)^6`$ への変形。
- `generatingFunction_coeff_eq_a365218` — 母関数の係数と `a365218` の
  係数表示が一致すること。
- `a365218_pos` — 係数表示から得られる全項の正値性。
- `generatingFunction_coeff_pos` — 元の母関数方程式から直接得られる全係数の正値性。
- `generatingFunction_unique` — 元の方程式の形式的冪級数解が高々一つであること。

解の存在そのものは別定理として構成していません。最終定理は、OEIS の方程式を
満たす任意の形式的冪級数について正値性を証明します。解析的な収束は一切使いません。

## ファイル

- `A365218Lean/Definitions.lean` — 核多項式と係数表示。
- `A365218Lean/LagrangeInversion.lean` — 形式的留数による Lagrange 反転の証明。
- `A365218Lean/GeneratingFunction.lean` — 元の母関数方程式と係数表示の橋渡し。
- `A365218Lean/CoefficientRecurrence.lean` — 微分恒等式と係数漸化式。
- `A365218Lean/Positivity.lean` — 比の帰納法と正値性。
- `A365218Lean/Main.lean` — 公開する最終定理。
- `A365218Lean/AxiomAudit.lean` — 公理監査。

## 自分で検証する方法

`lean-toolchain` と `lake-manifest.json` により Lean 4.27.0 と
Mathlib 4.27.0 を固定しています。`lakefile.toml` のあるプロジェクトルートで
次を実行してください。

```sh
lake update
lake exe cache get
lake build
lake env lean A365218Lean/AxiomAudit.lean
```

証明には `sorry`、`admit`、`sorryAx`、プロジェクト独自の公理を使用しません。
公理監査では Mathlib が通常使用する `propext`、`Classical.choice`、
`Quot.sound` が表示される場合があります。

## ディレクトリ非依存性

プロジェクト内の参照はすべて相対パスです。`lakefile.toml` を含むディレクトリ全体を
改名または移動しても、移動先をカレントディレクトリとして同じコマンドを実行できます。
