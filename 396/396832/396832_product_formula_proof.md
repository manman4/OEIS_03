# A396832 の積公式の証明

## 1. 定義と結論

<code>[n] = {1,2,...,n}</code> とする。A396832 の <code>a(n)</code> は、
<code>[n]</code> の置換

$$
p=(p(1),p(2),\ldots,p(n))
$$

で

$$
kp(k)\geq n-1\qquad(1\leq k\leq n)
$$

を満たすものの個数である。

証明する結論は、空積を 1 とする約束のもとで

$$
\boxed{a(0)=1,\qquad a(1)=1}
$$

および

$$
\boxed{\displaystyle
a(n)=\prod_{k=1}^{n}
\left(n-k+2-\left\lceil\frac{n-1}{k}\right\rceil\right)
\qquad(n\geq2)}
$$

である。従って OEIS 形式では

~~~text
a(n) = Product_{k=1..n} (n - k + 2 - ceiling((n-1)/k)) unless n = 1.
~~~

と書ける。

## 2. 例外的な小さい場合

### <code>n = 0</code>

空集合の置換は空置換ただ一つであり、条件も空である。従って

$$
a(0)=1.
$$

積公式の右辺も空積なので 1 である。従って積公式は
<code>n = 0</code> では成立する。

### <code>n = 1</code>

<code>[1]</code> の置換は <code>p(1)=1</code> ただ一つであり、

$$
1\cdot p(1)=1\geq0=n-1
$$

を満たす。従って

$$
a(1)=1.
$$

一方、積公式をそのまま代入すると

$$
1-1+2-\left\lceil\frac{0}{1}\right\rceil=2
$$

となるため、<code>n = 1</code> では成立しない。この例外が生じる
正確な理由は §8 で説明する。

以下では <code>n >= 2</code> とする。

## 3. 各位置で許される値

位置 <code>k</code> における条件は

$$
kp(k)\geq n-1.
$$

ここで <code>k >= 1</code> なので、これは

$$
p(k)\geq\frac{n-1}{k}
$$

と同値である。さらに <code>p(k)</code> は整数だから、

$$
p(k)\geq\left\lceil\frac{n-1}{k}\right\rceil
$$

と同値である。

位置 <code>k</code> で条件を満たす <code>[n]</code> の値全体を
<code>A_k</code> とする。<code>n >= 2</code> だから
<code>n-1 > 0</code> である。また
<code>k >= 1</code> より

$$
0<\frac{n-1}{k}\leq n-1.
$$

従って

$$
1\leq\left\lceil\frac{n-1}{k}\right\rceil\leq n-1.
$$

従って、<code>A_k</code> は <code>[n]</code> の中で途切れのない区間

$$
A_k=
\left[
\left\lceil\frac{n-1}{k}\right\rceil,
n
\right]\cap\mathbb Z
$$

であり、その要素数は

$$
|A_k|=n-\left\lceil\frac{n-1}{k}\right\rceil+1
$$

である。

## 4. 許容集合の入れ子構造

<code>1 <= i < k <= n</code> とする。<code>x in A_i</code> ならば

$$
ix\geq n-1.
$$

また <code>x in [n]</code> だから <code>x >= 1</code> であり、
<code>i < k</code> より

$$
kx\geq ix\geq n-1.
$$

従って <code>x in A_k</code> である。よって

$$
A_1\subseteq A_2\subseteq\cdots\subseteq A_n.
$$

この包含関係は、過去に使用した値がすべて現在の許容集合に
入っていることを保証する。

## 5. 各段階の選択肢数

位置 <code>1,2,...,n</code> をこの順に埋める。

位置 <code>k</code> を埋める直前には、位置
<code>1,...,k-1</code> に相異なる <code>k-1</code> 個の値が入っている。
各 <code>i < k</code> について

$$
p(i)\in A_i\subseteq A_k
$$

だから、使用済みの <code>k-1</code> 個の値はすべて
<code>A_k</code> に属する。

従って、位置 <code>k</code> で選べる未使用の値の個数は、
それ以前に具体的にどの値を選んだかによらず

$$
|A_k|-(k-1)
$$

である。§3 の要素数を代入すると

$$
\begin{aligned}
|A_k|-(k-1)
&=n-\left\lceil\frac{n-1}{k}\right\rceil+1-(k-1)\\
&=n-k+2-\left\lceil\frac{n-1}{k}\right\rceil.
\end{aligned}
$$

従って乗法原理により、すべての位置を埋める方法の個数は

$$
\prod_{k=1}^{n}
\left(n-k+2-\left\lceil\frac{n-1}{k}\right\rceil\right)
$$

となる。

より形式的には、<code>P_k</code> を、相異なる値
<code>x_1,...,x_k</code> で <code>x_i in A_i</code> を満たす列の集合と
する。<code>P_0</code> は空列だけからなるので <code>|P_0|=1</code> で
ある。上の議論により、<code>P_(k-1)</code> の各要素は、選択内容に
よらずちょうど

$$
n-k+2-\left\lceil\frac{n-1}{k}\right\rceil
$$

個の <code>P_k</code> の要素へ拡張される。しかも各
<code>P_k</code> の要素は、最後の値を削除すればただ一つの
<code>P_(k-1)</code> の要素へ戻る。従って

$$
|P_k|=|P_{k-1}|
\left(n-k+2-\left\lceil\frac{n-1}{k}\right\rceil\right).
$$

この等式を <code>k=1,...,n</code> について反復すると、上の積が
得られる。

## 6. 過不足や重複がないこと

上の逐次構成では、各位置 <code>k</code> に <code>A_k</code> の値を
置くので、得られる列は各不等式

$$
kp(k)\geq n-1
$$

を満たす。また、常に未使用の値を選ぶので、<code>n</code> 個の位置を
埋め終わった時点で <code>[n]</code> の <code>n</code> 個の値をすべて
ちょうど一度使っている。従って得られる列は確かに
<code>[n]</code> の置換である。

逆に、定義の条件を満たす任意の置換 <code>p</code> は、各位置で
<code>p(k) in A_k</code> となる。位置 <code>1,2,...,n</code> の順に
その値を読むと、上の逐次構成におけるただ一つの選択列が得られる。

従って逐次構成と条件を満たす置換との間には全単射があり、
過不足も重複もない。

## 7. 各因子が正であること

<code>1 <= k <= n</code> のとき、値 <code>n-k+1</code> について

$$
\begin{aligned}
k(n-k+1)-(n-1)
&=kn-k^2+k-n+1\\
&=(k-1)(n-k)+1\\
&\geq1.
\end{aligned}
$$

さらに <code>x >= n-k+1</code> ならば、<code>k >= 1</code> より

$$
kx\geq k(n-k+1)\geq n-1.
$$

従って

$$
n-k+1,n-k+2,\ldots,n
$$

という <code>k</code> 個の値はすべて <code>A_k</code> に属する。
よって <code>|A_k| >= k</code> であり、

$$
|A_k|-(k-1)\geq1.
$$

従って積公式の各因子は正整数である。

また、逆順置換

$$
p(k)=n-k+1\qquad(1\leq k\leq n)
$$

は各位置で

$$
kp(k)=k(n-k+1)\geq n-1
$$

を満たす。従って

$$
(n,n-1,\ldots,1)
$$

は常に条件を満たす置換である。これは、条件を満たす置換が
少なくとも一つ存在することの直接的な確認にもなっている。

## 8. <code>n = 1</code> だけが例外になる理由

一般に、位置 <code>k</code> の許容集合は正確には

$$
A_k=
[n]\cap
\left[
\left\lceil\frac{n-1}{k}\right\rceil,n
\right]\cap\mathbb Z
$$

である。

<code>n >= 2</code> では

$$
\left\lceil\frac{n-1}{k}\right\rceil\geq1
$$

だから、下端から <code>n</code> までの整数はすべて
<code>[n]</code> に入り、

$$
|A_k|=n-\left\lceil\frac{n-1}{k}\right\rceil+1
$$

と数えてよい。

しかし <code>n = 1</code> では下端が

$$
\left\lceil\frac{0}{1}\right\rceil=0
$$

となる。このとき、整数区間 <code>{0,1}</code> のうち
<code>[1]</code> に属するのは 1 だけであり、0 は置換の値として
使用できない。従って実際の許容集合は

$$
A_1=[1],\qquad |A_1|=1
$$

であるのに、式

$$
n-\left\lceil\frac{n-1}{k}\right\rceil+1
$$

は 0 まで数えて 2 を与えてしまう。これが
<code>n = 1</code> を除外しなければならない唯一の理由である。

## 9. 例: <code>n = 5</code>

この場合 <code>n-1=4</code> であり、各段階は次のようになる。

$$
\begin{array}{c|c|c|c}
k & \left\lceil 4/k\right\rceil & |A_k| & |A_k|-(k-1)\\
\hline
1 & 4 & 2 & 2\\
2 & 2 & 4 & 3\\
3 & 2 & 4 & 2\\
4 & 1 & 5 & 2\\
5 & 1 & 5 & 1
\end{array}
$$

従って

$$
a(5)=2\cdot3\cdot2\cdot2\cdot1=24.
$$

## 10. <code>396832_01.c</code> との関係

この積公式は定義から導かれる正しい定理である。一方、
<code>396832_01.c</code> は独立な計算方法を保つため、この積公式を
使用していない。

同プログラムは、定義から直接 0-1 行列

$$
M_{k,x}=1
\quad\Longleftrightarrow\quad
kx\geq n-1
$$

を作る。不等式を満たさない成分は 0 とする。そして、一般的な
Ryser 法でこの行列の permanent を計算している。
従って積公式はプログラムのアルゴリズムではなく、独立した数学的な
説明および出力検証として利用できる。

## 11. 結論

<code>n >= 2</code> では、許容集合の入れ子構造と乗法原理により

$$
\boxed{\displaystyle
a(n)=\prod_{k=1}^{n}
\left(n-k+2-\left\lceil\frac{n-1}{k}\right\rceil\right)}.
$$

<code>n = 0</code> では空積として同じ式が成立する。
<code>n = 1</code> では実際の値は <code>a(1)=1</code> だが、積の
右辺は 2 になる。従って

~~~text
a(n) = Product_{k=1..n} (n - k + 2 - ceiling((n-1)/k)) unless n = 1.
~~~

は正しい。

参考: [OEIS A396832](https://oeis.org/A396832)
