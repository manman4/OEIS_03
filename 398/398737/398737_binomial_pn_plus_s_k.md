# a(n) = Σ binomial(pn+s, k) a(n−k) の母関数と性質

## 主張

$p \ge 1$、$s$ を整数とし（$p + s \ge 0$）、数列 $a(n)$ を

$$
a(0) = 1, \qquad a(n) = \sum_{k=1}^{n} \binom{pn+s}{k}\, a(n-k) \quad (n \ge 1)
$$

で定める。母関数を $A(x) = \sum_{n \ge 0} a(n) x^n$ とする。

1. **母関数.**
   $$
   A(x) = \frac{1-(p-1)x}{(1+x)^{s+1}}\left(2A\!\left(\frac{x}{(1+x)^p}\right) - 1\right).
   $$
2. **割り切れる性質（$s = 0, -1$）.**
   - $s = 0$ のとき、$n \ge 1$ で $p \mid a(n)$ であり、$\gcd_{n \ge 1} a(n) = p$ である。
   - $s = -1$、$p \ge 2$ のとき、$n \ge 1$ で $(p-1) \mid a(n)$ であり、$\gcd_{n \ge 1} a(n) = p-1$ である。

## 特に s = −1 の場合

$s = -1$ では分母の $(1+x)^{s+1}$ が消えて、

$$
A(x) = \bigl(1-(p-1)x\bigr)\left(2A\!\left(\frac{x}{(1+x)^p}\right) - 1\right)
$$

となる。

**例（p = 5）.** $a(n) = \sum_{k=1}^{n} \binom{5n-1}{k} a(n-k)$ の母関数は

$$
A(x) = (1-4x)\left(2A\!\left(\frac{x}{(1+x)^5}\right) - 1\right)
$$

で、$n \ge 1$ で $4 \mid a(n)$ である。

OEIS 形式では次のように書ける。

> a(0) = 1; a(n) = Sum_{k=1..n} binomial(5*n-1,k) * a(n-k).
>
> G.f. A(x) satisfies A(x) = (1-4*x) * (2*A(x/(1+x)^5) - 1).
>
> 4 | a(n) for n > 0.

## 例

**s = 0（binomial(pn, k)）**

| $p$ | 母関数 | 数列 |
|---|---|---|
| 1 | $A(x) = \dfrac{1}{1+x}\bigl(2A(\tfrac{x}{1+x}) - 1\bigr)$ | 1, 1, 3, 13, 75, 541, …（Fubini 数） |
| 2 | $A(x) = \dfrac{1-x}{1+x}\bigl(2A(\tfrac{x}{(1+x)^2}) - 1\bigr)$ | 1, 2, 14, 134, 1646, 24842, … |
| 3 | $A(x) = \dfrac{1-2x}{1+x}\bigl(2A(\tfrac{x}{(1+x)^3}) - 1\bigr)$ | 1, 3, 33, 489, 9201, 211473, … |

**s = −1（binomial(pn−1, k)）**

| $p$ | 母関数 | 数列 | 割り切る数 |
|---|---|---|---|
| 2 | $A(x) = (1-x)\bigl(2A(\tfrac{x}{(1+x)^2}) - 1\bigr)$ | 1, 1, 6, 50, 546, 7470, 123354, … | 1 |
| 3 | $A(x) = (1-2x)\bigl(2A(\tfrac{x}{(1+x)^3}) - 1\bigr)$ | 1, 2, 20, 272, 4752, 102564, … | 2 |
| 4 | $A(x) = (1-3x)\bigl(2A(\tfrac{x}{(1+x)^4}) - 1\bigr)$ | 1, 3, 42, 792, 19020, 560766, … | 3 |
| 5 | $A(x) = (1-4x)\bigl(2A(\tfrac{x}{(1+x)^5}) - 1\bigr)$ | 1, 4, 72, 1736, 53048, 1983024, 88048640, … | 4 |
| 6 | $A(x) = (1-5x)\bigl(2A(\tfrac{x}{(1+x)^6}) - 1\bigr)$ | 1, 5, 110, 3230, 119830, 5425900, … | 5 |

## 母関数の証明（ラグランジュ反転）

$z = \dfrac{x}{(1+x)^p}$ とおくと $x = z\,\varphi(x)$、$\varphi(x) = (1+x)^p$ と書ける。べき級数 $F$ に対して

$$
\sum_{n \ge 0} z^n\, [x^n]\, F(x)\varphi(x)^n = \frac{F(x)}{1 - z\varphi'(x)}
$$

が成り立つ。ここで

$$
z\varphi'(x) = \frac{x}{(1+x)^p} \cdot p(1+x)^{p-1} = \frac{px}{1+x},
\qquad
1 - z\varphi'(x) = \frac{1-(p-1)x}{1+x}
$$

である。

$F(x) = A(x)(1+x)^s$ とすると、$[x^k](1+x)^{pn+s} = \binom{pn+s}{k}$ より

$$
c(n) := [x^n]\, A(x)(1+x)^{pn+s} = \sum_{k=0}^{n} \binom{pn+s}{k}\, a(n-k)
$$

となる。よって

$$
\sum_{n \ge 0} c(n)\, z^n = \frac{(1+x)^{s+1}A(x)}{1-(p-1)x}
$$

が成り立つ。

$n \ge 1$ では、$k = 0$ の項 $a(n)$ に漸化式の和 $a(n)$ が加わるので $c(n) = 2a(n)$ である。$n = 0$ では $c(0) = a(0) = 1$ である。まとめると $c(n) = 2a(n) - [n=0]$ なので、

$$
\frac{(1+x)^{s+1}A(x)}{1-(p-1)x} = 2A(z) - 1
$$

となり、$A(x)$ について解くと主張 1 の式が得られる。

**一意性.** この関数方程式を満たす級数は一つしかない。$z = x + O(x^2)$ なので、右辺の $x^n$ の係数は $2a(n) + (\text{$a(0),\dots,a(n-1)$ で決まる量})$ となり、$a(n)$ が順に決まるからである。したがって、母関数の式と漸化式は同じ数列を定める。 $\blacksquare$

## 割り切れる性質の証明

**補題.** $n \ge 1$ のとき

$$
\binom{pn}{n} = p\binom{pn-1}{n-1},
\qquad
\binom{pn-1}{n} = (p-1)\binom{pn-1}{n-1}.
$$

1 つ目は $\binom{N}{n} = \dfrac{N}{n}\binom{N-1}{n-1}$ に $N = pn$ を代入したものである。2 つ目は

$$
\frac{\binom{pn-1}{n}}{\binom{pn-1}{n-1}}
= \frac{(n-1)!\,(pn-n)!}{n!\,(pn-n-1)!}
= \frac{(p-1)n}{n} = p-1
$$

から得られる。

**証明.** $d = p$（$s = 0$ のとき）または $d = p-1$（$s = -1$ のとき）とする。$n \ge 1$ について強い帰納法を使う。$1 \le m < n$ を満たすすべての $m$ で $d \mid a(m)$ が成り立つと仮定する（$n = 1$ のときは仮定は空である）。

$$
a(n) = \sum_{k=1}^{n-1} \binom{pn+s}{k}\, a(n-k) \;+\; \binom{pn+s}{n}\, a(0).
$$

1. **$1 \le k \le n-1$ の項.** $1 \le n-k \le n-1$ なので、帰納法の仮定から $d \mid a(n-k)$ である。
2. **$k = n$ の項.** $a(0) = 1$ なので、この項は $\binom{pn+s}{n}$ である。補題より $d$ の倍数である。

よって $d \mid a(n)$ である。

**最大であること.** $a(1) = \binom{p+s}{1} = p+s = d$ なので、$n \ge 1$ のすべての $a(n)$ を割り切る数は $d$ 以下である。したがって $\gcd_{n \ge 1} a(n) = d$ である。 $\blacksquare$

| 漸化式 | $k = n$ の項 | 割り切る数 |
|---|---|---|
| $\binom{pn}{k}$ | $\binom{pn}{n} = p\binom{pn-1}{n-1}$ | $p$ |
| $\binom{pn-1}{k}$ | $\binom{pn-1}{n} = (p-1)\binom{pn-1}{n-1}$ | $p-1$ |

## binomial(tn+k+s′, k) の族との関係

$$
b(0) = 1, \qquad b(n) = \sum_{k=1}^{n} \binom{tn+k+s'}{k}\, b(n-k)
$$

の母関数は

$$
B(x) = (1-x)^{s'}\bigl(1-(t+1)x\bigr)\Bigl(2B\bigl(x(1-x)^t\bigr) - 1\Bigr)
$$

を満たす。この族で $t = -p$ とおくと、binomial(pn+s, k) の族と対応する。

1. 二項係数の上を負にする公式 $\binom{r}{k} = (-1)^k\binom{k-r-1}{k}$ を $r = -pn + k + s'$ に使うと、$\binom{-pn+k+s'}{k} = (-1)^k\binom{pn-s'-1}{k}$ となる。つまり $s = -s'-1$ と対応する。
2. よって $b(n) = \sum_{k=1}^{n} (-1)^k \binom{pn+s}{k}\, b(n-k)$ である。
3. $a(n) = (-1)^n b(n)$ とおくと、符号 $(-1)^n (-1)^k (-1)^{n-k} = 1$ が消えて、$a(n) = \sum_{k=1}^{n} \binom{pn+s}{k}\, a(n-k)$ となる。
4. 母関数では $A(x) = B(-x)$ であり、$x \to -x$ とすると
   - $(1-x)^{s'}\bigl(1-(t+1)x\bigr) \to (1+x)^{s'}\bigl(1-(p-1)x\bigr) = \dfrac{1-(p-1)x}{(1+x)^{s+1}}$
   - $x(1-x)^t \to -\dfrac{x}{(1+x)^p}$、つまり $B\!\left(-\dfrac{x}{(1+x)^p}\right) = A\!\left(\dfrac{x}{(1+x)^p}\right)$

   となり、主張 1 の式と一致する。

| binomial(pn+s, k) の族 | 対応する族 |
|---|---|
| $s = 0$：$\binom{pn}{k}$ | $\binom{tn+k-1}{k}$（$s' = -1$）、$t = -p$ |
| $s = -1$：$\binom{pn-1}{k}$ | $\binom{tn+k}{k}$（$s' = 0$）、$t = -p$ |

割り切る数も対応している。$\binom{tn+k}{k}$ の族では $t+1 = 1-p$、$\binom{tn+k-1}{k}$ の族では $t = -p$ で割り切れ、符号を除いてそれぞれ $p-1$、$p$ になる。

## 補足

- 母関数の式は、$p = 1, \dots, 6$、$s = -2, \dots, 2$（$p + s \ge 0$）について $x^{14}$ の係数まで整数演算で一致することを確かめた。
- $s = -1$ の $\gcd_{n \ge 1} a(n) = p-1$ は、$p = 2, \dots, 8$ について $n = 49$ まで数値計算で確かめた。
- $s = 0$ の $\gcd_{n \ge 1} a(n) = p$ は、$p = 1, \dots, 8$ について $n = 59$ まで数値計算で確かめた。
