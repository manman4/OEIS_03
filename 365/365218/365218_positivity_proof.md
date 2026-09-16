# A365218 の全項正値性

[OEIS A365218](https://oeis.org/A365218) の母関数を

```math
A(x)=\sum_{n\geq 0}a(n)x^n
```

とする。これは次の方程式によって定義されている。

```math
A(x)=1+\frac{xA(x)^6}{1+xA(x)^6}.
```

この文書では、OEIS に conjecture として記載されている次の主張を証明する。

```math
a(n)\gt 0\qquad(n\geq 0).
```

## 1. Lagrange 反転による係数表示

```math
B(x)=A(x)-1
```

と置く。母関数方程式を整理すると、

```math
B=x(1-B)(1+B)^6
```

を得る。したがって、Lagrange の反転公式により、$`n\geq1`$ に対して

```math
\begin{aligned}
a(n)
&=[x^n]B(x) \\
&=\frac{1}{n}[t^{n-1}]((1-t)(1+t)^6)^n \\
&=\frac{1}{n}[t^{n-1}](1-t)^n(1+t)^{6n}.
\end{aligned}
```

特に、次の有限和表示も得られる。

```math
a(n)=\frac{1}{n}\sum_{j=0}^{n-1}(-1)^j
\binom{n}{j}\binom{6n}{n-1-j}.
```

この有限和は交代和なので、これだけから正値性が直ちに分かるわけではない。以下では係数に関する漸化式を用いる。

## 2. 補助係数の漸化式

$`n`$ を固定し、

```math
C_n(t)=(1-t)^n(1+t)^{6n}=\sum_{k\geq0}c_k t^k
```

と置く。上の係数表示から、$`n\geq1`$ に対して

```math
a(n)=\frac{c_{n-1}}{n}
```

である。

$`C_n(t)`$ を微分すると、

```math
(1-t^2)C_n'(t)=n(5-7t)C_n(t)
```

を得る。両辺の係数を比較することにより、

```math
c_0=1,\qquad c_1=5n
```

および、$`k\geq1`$ に対する漸化式

```math
(k+1)c_{k+1}=5n c_k-(7n-k+1)c_{k-1}
```

が得られる。

## 3. 比に関する帰納法

$`n\geq2`$ とする。次の命題を $`k`$ に関する帰納法で証明する。

```math
P(k):\qquad
c_{k-1}\gt0,\quad c_k\gt0,\quad
r_k:=\frac{c_k}{c_{k-1}}\gt\frac{2n}{k}.
```

証明する範囲は $`1\leq k\leq n-1`$ である。

### 基底

$`c_0=1`$ および $`c_1=5n`$ だから、

```math
r_1=\frac{c_1}{c_0}=5n\gt2n.
```

したがって $`P(1)`$ が成立する。

### 帰納段階

$`1\leq k\leq n-2`$ とし、$`P(k)`$ が成立すると仮定する。漸化式を正数 $`c_k`$ で割ると、

```math
r_{k+1}
=\frac{1}{k+1}
(5n-\frac{7n-k+1}{r_k})
```

となる。帰納法の仮定から

```math
r_k\gt\frac{2n}{k}\gt0,\qquad
\frac{1}{r_k}\lt\frac{k}{2n}.
```

よって、

```math
r_{k+1}
\gt
\frac{1}{k+1}
(5n-\frac{k(7n-k+1)}{2n}).
```

ここで

```math
f(k)=k(7n-k+1)
```

と置くと、

```math
f(k+1)-f(k)=7n-2k\gt0
```

である。したがって $`1\leq k\leq n-2`$ の範囲では、

```math
\begin{aligned}
k(7n-k+1)
&\leq(n-2)(6n+3) \\
&=6n^2-9n-6 \\
&\lt6n^2.
\end{aligned}
```

従って、

```math
\begin{aligned}
r_{k+1}
&\gt\frac{1}{k+1}
(5n-\frac{6n^2}{2n}) \\
&=\frac{2n}{k+1} \\
&\gt0.
\end{aligned}
```

さらに $`c_k\gt0`$ なので、

```math
c_{k+1}=r_{k+1}c_k\gt0.
```

これにより $`P(k+1)`$ が成立する。なお、$`n=2`$ の場合は帰納段階の範囲が空であり、基底だけで必要な主張が得られる。

以上の帰納法により、

```math
r_k\gt\frac{2n}{k}\gt0
\qquad(1\leq k\leq n-1)
```

が証明された。特に $`c_{n-1}\gt0`$ である。

## 4. 結論

$`n\geq2`$ では、比の下界を掛け合わせることにより、さらに

```math
\begin{aligned}
c_{n-1}
&=c_0\prod_{k=1}^{n-1}r_k \\
&\gt\prod_{k=1}^{n-1}\frac{2n}{k} \\
&=\frac{(2n)^{n-1}}{(n-1)!}
\end{aligned}
```

を得る。従って、

```math
a(n)=\frac{c_{n-1}}{n}
\gt\frac{(2n)^{n-1}}{n!}
\gt0.
```

$`a(0)=1`$ は母関数方程式から分かり、$`a(1)=1`$ は Lagrange 反転による係数表示から分かる。以上により、すべての $`n\geq0`$ に対して

```math
\boxed{a(n)\gt0}
```

が成立する。
