# A330432 の積公式の証明

## 1. 定義と結論

\([n]=\{1,2,\ldots,n\}\) とする。A330432 は、\([n]\) の置換

\[
\sigma=(\sigma(1),\sigma(2),\ldots,\sigma(n))
\]

で

\[
k\sigma(k)\geq n\qquad(1\leq k\leq n)
\]

を満たすものの個数 \(a(n)\) である。

結論は

\[
\boxed{\displaystyle
a(0)=1,\qquad
a(n)=\prod_{k=1}^{n}
\left(n-k+2-\left\lceil\frac{n}{k}\right\rceil\right)
\quad(n\geq1)}
\]

である。以下、この公式を定義から証明する。

## 2. 各位置で許される値

\(n\geq1\) を固定する。位置 \(k\) における条件は

\[
k\sigma(k)\geq n
\]

である。\(\sigma(k)\) は整数なので、これは

\[
\sigma(k)\geq\left\lceil\frac{n}{k}\right\rceil
\]

と同値である。

そこで、位置 \(k\) で許される値の集合を

\[
A_k=
\left\{
\left\lceil\frac{n}{k}\right\rceil,
\left\lceil\frac{n}{k}\right\rceil+1,
\ldots,n
\right\}
\]

と定める。このとき

\[
|A_k|=n-\left\lceil\frac{n}{k}\right\rceil+1.
\]

したがって、求める置換は

\[
\sigma(k)\in A_k\qquad(1\leq k\leq n)
\]

を満たす置換にほかならない。

## 3. 許容集合は入れ子になる

\(k\) が増加すると \(n/k\) は減少するので、

\[
\left\lceil\frac{n}{k+1}\right\rceil
\leq
\left\lceil\frac{n}{k}\right\rceil.
\]

よって

\[
A_1\subseteq A_2\subseteq\cdots\subseteq A_n.
\]

この入れ子構造が積公式の本質である。

## 4. 位置を順番に埋める

位置 \(1,2,\ldots,n\) をこの順に埋める。

位置 \(k\) を埋める直前に、位置 \(1,\ldots,k-1\) には相異なる
\(k-1\) 個の値がすでに入っているとする。各 \(i<k\) について

\[
\sigma(i)\in A_i\subseteq A_k
\]

だから、使用済みの \(k-1\) 個の値はすべて \(A_k\) に属している。

したがって、位置 \(k\) で選べる未使用の値の個数は、どの有効な
部分置換から出発したかによらず

\[
|A_k|-(k-1)
\]

である。\(|A_k|\) を代入すると

\[
\begin{aligned}
|A_k|-(k-1)
&=n-\left\lceil\frac{n}{k}\right\rceil+1-(k-1)\\
&=n-k+2-\left\lceil\frac{n}{k}\right\rceil.
\end{aligned}
\]

従って乗法原理により、すべての位置を埋める方法の総数は

\[
\prod_{k=1}^{n}
\left(n-k+2-\left\lceil\frac{n}{k}\right\rceil\right)
\]

である。

この構成では各位置に許容集合から値を選び、同じ値を二度選ばない。
\(n\) 個の位置を埋め終わると \([n]\) の \(n\) 個の値をすべて
ちょうど一度使っているので、得られる列は確かに \([n]\) の置換である。
逆に、条件を満たす置換は位置を順に読むことでこの構成から一意に
得られる。

したがって過不足も重複もなく、積公式が証明された。

## 5. 各因子が正であること

積の各因子が負または 0 にならないことも直接確認できる。
\(A_k\) は少なくとも

\[
\{n-k+1,n-k+2,\ldots,n\}
\]

を含む。実際、この集合の最小値 \(n-k+1\) について

\[
\begin{aligned}
k(n-k+1)-n
&=kn-k^2+k-n\\
&=(k-1)(n-k)\\
&\geq0
\end{aligned}
\]

だからである。従って \(|A_k|\geq k\) であり、

\[
|A_k|-(k-1)\geq1.
\]

よって積のすべての因子は正整数である。なお、この議論は逆順置換

\[
(n,n-1,\ldots,1)
\]

が常に条件を満たすことも示している。

## 6. 両端の値

### \(\sigma(1)=n\)

位置 1 では

\[
\sigma(1)\geq\left\lceil\frac{n}{1}\right\rceil=n.
\]

一方 \(\sigma(1)\leq n\) なので、

\[
\boxed{\sigma(1)=n}.
\]

これは \(A_1=\{n\}\) と同じことである。

### \(\sigma(n)=1\)

値 1 を位置 \(k\) に置けるための条件は

\[
k\cdot1\geq n.
\]

\(1\leq k\leq n\) の範囲では、これは \(k=n\) の場合にのみ成立する。
置換は値 1 を必ず一度使うので、

\[
\boxed{\sigma(n)=1}.
\]

従って OEIS のコメント

~~~text
sigma(1) = n and sigma(n) = 1.
~~~

も定義から直ちに従う。

## 7. \(n=0\) と OFFSET

\(n=0\) の場合、空集合の置換は空置換ただ一つであり、条件の添字範囲も
空である。従って

\[
a(0)=1.
\]

積公式の右辺も空積を 1 と定めれば \(n=0\) にそのまま拡張できる。
よって A330432 の OFFSET 0 と初項 \(a(0)=1\) は整合している。

\(n=1\) では因子は

\[
1-1+2-\left\lceil\frac11\right\rceil=1
\]

であり、\(a(1)=1\) となる。

## 8. 例: \(n=5\)

各位置で残る選択肢数は

\[
\begin{array}{c|c|c|c}
k & \lceil 5/k\rceil & |A_k| & |A_k|-(k-1)\\
\hline
1 & 5 & 1 & 1\\
2 & 3 & 3 & 2\\
3 & 2 & 4 & 2\\
4 & 2 & 4 & 1\\
5 & 1 & 5 & 1
\end{array}
\]

なので

\[
a(5)=1\cdot2\cdot2\cdot1\cdot1=4.
\]

これは A330432 の値 `a(5)=4` と一致する。

## 9. 実装との対応

整数除算だけを使う場合、

~~~text
ceil(n/k) = n/k + (n mod k != 0 ? 1 : 0)
~~~

と安全に計算できる。従って 330432_01.c は各 \(k\) について

~~~text
factor = n - k + 2 - ceil(n/k)
result *= factor
~~~

を実行している。

必要な演算は \(n\) 回の正整数乗算だけであり、置換の全探索や推測による
枝刈りは使用していない。出力が機械整数を超えるため、実装では
基数 \(10^9\) の可変長非負整数を使って積を厳密に保持している。

## 10. 結論

A330432 の定義を満たす置換の個数は、許容集合の入れ子構造により

\[
\boxed{\displaystyle
a(0)=1,\qquad
a(n)=\prod_{k=1}^{n}
\left(n-k+2-\left\lceil\frac{n}{k}\right\rceil\right)
\quad(n\geq1)}
\]

で厳密に与えられる。

参考: [OEIS A330432](https://oeis.org/A330432)
