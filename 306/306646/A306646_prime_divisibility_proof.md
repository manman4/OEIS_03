# A306646 の素数による整除性

[A306646](https://oeis.org/A306646) を、各列の母関数が

$$
\sum_{n\geq 0} A(n,k)x^n
=\frac{k+1-x^k}{1-x^k-x^{k+1}}
\qquad (k\geq 1)
$$

である配列とする。

## 定理

素数 $p$ と整数 $k\geq 2$ に対して

$$
\boxed{p\mid A(p,k)}
$$

が成り立つ。

## 証明

A306646には、$n>0$ に対して

$$
A(n,k)
=n\sum_{j=1}^{\lfloor n/k\rfloor}
 \frac{1}{j}\binom{j}{n-kj}
$$

という公式がある。ここで $n=p$ とすると

$$
A(p,k)
=p\sum_{j=1}^{\lfloor p/k\rfloor}
 \frac{1}{j}\binom{j}{p-kj}.
$$

$k\geq 2$ なので、和に現れるすべての $j$ は

$$
1\leq j\leq\left\lfloor\frac{p}{k}\right\rfloor
\leq\left\lfloor\frac{p}{2}\right\rfloor<p
$$

を満たす。したがって $p\nmid j$ であり、各分母 $j$ は法 $p$ で
可逆である。よって括弧内の和は $p$-進整数であり、その外側に
因子 $p$ があるから

$$
A(p,k)\equiv 0\pmod p.
$$

$p<k$ の場合は和が空で $A(p,k)=0$ となるので、この場合にも結論は
成立する。

$$
\square
$$

## 注記：$k=1$ の場合

$k=1$ の場合には和に $j=p$ の項が現れ、その寄与は

$$
\frac{p}{p}\binom{p}{0}=1
$$

である。それ以外の $j<p$ の項はすべて $p$ の倍数なので、

$$
A(p,1)\equiv 1\pmod p.
$$

したがって $k=1$ では整除性は成立せず、定理の仮定 $k\geq 2$ は
必要である。


