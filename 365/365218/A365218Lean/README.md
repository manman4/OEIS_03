# A365218 の全項正値性 — Lean 4 形式化

[A365218](https://oeis.org/A365218) の母関数 $`A(x)`$ は、

```math
A(x)=1+\frac{xA(x)^6}{1+xA(x)^6}
```

を満たします。Lagrange 反転公式により、$`n\geq1`$ では

```math
a(n)=\frac{1}{n}[t^{n-1}](1-t)^n(1+t)^{6n}
```

です。このプロジェクトは、この係数表示から次の正値性を Lean 4 と Mathlib で証明します。

```lean
theorem a365218_pos (n : ℕ) :
    0 < a365218 n
```

## 証明の概要

固定した $`n`$ に対して、

```math
C_n(t)=(1-t)^n(1+t)^{6n}=\sum_{k\geq0}c_k t^k
```

と置きます。多項式恒等式

```math
(1-t^2)C_n'(t)=n(5-7t)C_n(t)
```

を Lean 上で証明し、係数比較から

```math
(k+1)c_{k+1}=5nc_k-(7n-k+1)c_{k-1}
```

を導きます。

次に、$`1\leq k\leq n-1`$ において

```math
c_k\gt0,\qquad 2n c_{k-1}\lt k c_k
```

を $`k`$ に関する帰納法で証明します。これは比を使えば

```math
\frac{c_k}{c_{k-1}}\gt\frac{2n}{k}
```

に対応する不等式です。帰納段階では、

```math
k(7n-k+1)\lt6n^2
```

を使用します。最終的に $`c_{n-1}\gt0`$ から $`a(n)\gt0`$ が従います。

## 形式化の範囲

`Definitions.lean` の `a365218` は、上記の Lagrange 係数表示を有理数値の関数として定義しています。`coefficientPolynomial_eq_product` により、Lean 内の核の累乗が実際に

```math
(1-t)^n(1+t)^{6n}
```

と等しいことも証明しています。

母関数方程式から Lagrange 係数表示を導く部分は形式化していません。従って、形式化済みの範囲は「Lagrange 係数表示から全項正値性まで」です。また、係数表示が整数値になることは、この正値性証明には不要なので形式化していません。

## ファイル

- `A365218Lean/Definitions.lean` — 係数多項式と A365218 の定義
- `A365218Lean/CoefficientRecurrence.lean` — 微分恒等式と係数漸化式
- `A365218Lean/Positivity.lean` — 二次不等式、帰納法、全項正値性
- `A365218Lean/Main.lean` — 公開定理の入口
- `A365218Lean/AxiomAudit.lean` — 使用公理の監査

## ディレクトリ非依存性

プロジェクト内の参照はすべて相対パスです。`lakefile.toml` を含むプロジェクトルート全体を任意の場所へ移動しても、同じ手順でビルドできます。Lean、Mathlib、および依存パッケージの revision は `lean-toolchain` と `lake-manifest.json` に固定されています。

## 検証方法

Lean 4.27.0 と Mathlib 4.27.0 を使用します。`lakefile.toml` のあるプロジェクトルートをカレントディレクトリとして、次を実行します。

```sh
lake update
lake exe cache get
lake build
lake env lean A365218Lean/AxiomAudit.lean
```

証明には `sorry`、`admit`、`sorryAx`、およびプロジェクト独自の公理を使用しません。公理監査では、Lean/Mathlib が通常使用する標準公理が表示されることがあります。
