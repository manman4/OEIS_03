# A306646 の素数可除性 — Lean 4 形式化

[A306646](https://oeis.org/A306646) の列 $k$ は、母関数

$$
\frac{k+1-x^k}{1-x^k-x^{k+1}}
$$

の係数列です。このプロジェクトでは、OEIS に記載された有限和表示を定義として、次を Lean 4 と Mathlib で証明します。

```lean
theorem prime_dvd_a306646 {p k : ℕ}
    (hp : Nat.Prime p) (hk : 2 ≤ k) :
    p ∣ a306646 p k
```

つまり、$p$ が素数、$k \ge 2$ ならば $p \mid A(p,k)$ です。

## 形式化の範囲

`Definitions.lean` の `a306646` は、$n > 0$ に対する係数公式

$$
A(n,k)
=n\sum_{j=1}^{\lfloor n/k\rfloor}
  \frac{1}{j}\binom{j}{n-kj}
$$

を自然数上で定義しています。`CoefficientFormula.lean` では、和の範囲内の各 $j$ について

$$
j\mid n\binom{j}{n-kj}
$$

を証明しているため、Lean の自然数除算が切り捨てを起こしていないことも形式的に保証されます。

このプロジェクトが形式化するのは、上の有限和表示から素数可除性を導く部分です。母関数から有限和表示を導く代数計算そのものは形式化していません。

## ファイル

- `A306646Lean/Definitions.lean` — 係数公式による配列と各項の定義
- `A306646Lean/CoefficientFormula.lean` — 各除算が完全除算であることの証明
- `A306646Lean/PrimeDivisibility.lean` — 素数可除性の証明
- `A306646Lean/Main.lean` — 公開定理の入口
- `A306646Lean/AxiomAudit.lean` — 使用公理の監査

すべてのプロジェクト内参照は相対パスなので、`lean` ディレクトリ全体を別の場所へ移動しても利用できます。

## 検証方法

Lean 4.27.0 と Mathlib 4.27.0 を使用します。

```sh
cd lean
lake update
lake exe cache get
lake build
lake env lean A306646Lean/AxiomAudit.lean
```

証明には `sorry` とカスタム公理を使用していません。公理監査には Lean/Mathlib が通常使用する `propext`、`Classical.choice`、`Quot.sound` が表示されます。
