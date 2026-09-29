# A374275 の11整除性 — Lean 4 形式化

このプロジェクトは、[A374275](https://oeis.org/A374275) の定義から
次の整除性を Lean 4 で無条件に形式化します。

```lean
theorem eleven_dvd_a374275 {n : ℕ} (hn : 0 < n) :
    11 ∣ a374275 n
```

## 形式化の範囲

`Definitions.lean` の `solutions k` は、0以上 `k` 以下の
`x,y` を列挙し、次の条件を満たす組だけを残します。

```math
0\le x\le y,\qquad x^2+3xy+y^2=k
```

方程式を満たす非負整数解では必ず `x ≤ k`、`y ≤ k` となることを
`mem_solutions_iff` で証明しているため、この有限範囲は探索上限ではなく
OEISの解集合そのものです。

`a374275 n` は、その解数を初めて取る最小の `k` として定義されます。
定義自体には該当する `k` がない場合の既定値0がありますが、正の `n`
について次を証明しているため、公開定理では既定値を使いません。

```math
R(11^{2n-1})=n
```

Leanで形式化済みなのは次の部分です。

- OEISどおりの解集合と表現数
- 最小値の定義と最小性
- `a374275 1 = 0`
- 黄金整数環 $`\mathbb Z[(1+\sqrt 5)/2]`$ のユークリッド整域性
- 二次形式の解とノルム類の共役軌道との全単射
- 表現数が2以上なら非自己共役な既約因子が存在すること
- 11以外の因子対を11の因子対へ移すと、表現数を保ったまま値が小さくなること
- $`11^{2n-1}`$ のノルム類と共役軌道を完全に数え、各正の表現数が存在すること
- 最小性による降下と、正の添字における11整除性

置換性 `ElevenReplacement` は仮定ではなく
`GoldenRing.elevenReplacement` として証明済みです。

## ファイル

- `A374275Lean/Definitions.lean` — 二次形式、有限解集合、表現数、最小値
- `A374275Lean/Descent.lean` — 11への置換特性から整除性を導く証明
- `A374275Lean/GoldenRing.lean` — 黄金整数環のユークリッド整域性
- `A374275Lean/GoldenOrbits.lean` — 解とノルム類の共役軌道との全単射
- `A374275Lean/GoldenReplacement.lean` — 11への置換特性の無条件証明
- `A374275Lean/GoldenExistence.lean` — 全正添字の存在証明
- `A374275Lean/Main.lean` — 公開定理の入口
- `A374275Lean/AxiomAudit.lean` — 使用公理の監査

## ビルド

この `lean` ディレクトリを任意の場所へ移動した後も、そのディレクトリを
カレントディレクトリとして次を実行できます。

```sh
lake update
lake exe cache get
lake build
lake env lean A374275Lean/AxiomAudit.lean
```

Lean 4.27.0 と Mathlib 4.27.0 を使用します。依存関係は
`lakefile.toml`、`lean-toolchain`、`lake-manifest.json` で
固定されています。プロジェクトファイルに絶対パスへの依存はありません。
`.lake/` は生成キャッシュなので移動対象に含める必要はありません。

`GoldenRing.lean` のユークリッド整域構成は、Barinder S. Banwait による
Ramanujan--Nagell 形式化の smart-rounding の構成を、判別式5の実二次環へ
適用したものです。該当箇所の帰属はソース冒頭にも記載しています。
