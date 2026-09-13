# A306646 の素数による整除性

[A306646](https://oeis.org/A306646) を、各列の母関数が

```math
\sum_{n\geq 0} A(n,k)x^n
=\frac{k+1-x^k}{1-x^k-x^{k+1}}
\qquad (k\geq 1)
```

である配列とする。

## 定理

素数 $p$ と整数 $k\geq 2$ に対して

```math
\boxed{p\mid A(p,k)}
```

が成り立つ。

## 証明

A306646には、$n>0$ に対して

```math
A(n,k)
=n\sum_{j=1}^{\lfloor n/k\rfloor}
 \frac{1}{j}\binom{j}{n-kj}
```

という公式がある。ここで $n=p$ とすると

```math
A(p,k)
=p\sum_{j=1}^{\lfloor p/k\rfloor}
 \frac{1}{j}\binom{j}{p-kj}.
```

$k\geq 2$ なので、和に現れるすべての $j$ は

```math
1\leq j\leq\left\lfloor\frac{p}{k}\right\rfloor
\leq\left\lfloor\frac{p}{2}\right\rfloor<p
```

を満たす。したがって $p\nmid j$ であり、各分母 $j$ は法 $p$ で
可逆である。よって括弧内の和は $p$-進整数であり、その外側に
因子 $p$ があるから

```math
A(p,k)\equiv 0\pmod p.
```

$p<k$ の場合は和が空で $A(p,k)=0$ となるので、この場合にも結論は
成立する。$\square$

## なぜ $k=1$ を除くのか

$k=1$ の場合には和に $j=p$ の項が現れる。この項は

```math
p\cdot\frac{1}{p}\binom{p}{0}=1
```

であり、それ以外の $j<p$ の項は $p$ の倍数である。したがって

```math
A(p,1)\equiv 1\pmod p,
```

となり、$k=1$ では主張は成立しない。


