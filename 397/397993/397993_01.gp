a_vector(n) = my(v=vector(n)); v[1]=1; for(i=1, n-1, v[i+1]=sum(j=1, i, 2^(i-j)*abs(stirling(i+1, j, 1))*v[j])); v;
a_vector(30)

\\ a(1) = 1; a(n+1) = Sum_{k=1..n} 2^(n-k) * |Stirling1(n+1,k)| * a(k).
\\ a(1) = 1; a(n) = Sum_{k=1..n-1} 2^(n-1-k) * |Stirling1(n,k)| * a(k).
a(n) = if(n==1, 1, sum(k=1, n-1, 2^(n-1-k) * abs(stirling(n, k, 1)) * a(k)));
for(n=1, 15, print1(a(n),", "));

\\ ------------------------------------------------------------
\\ A(-log(1-2*x)/2) = 3*A(x) - 2*x の確認
\\ ------------------------------------------------------------

N=15;
\\ x^N まで計算する。
X=x+O(x^(N+1));

A=sum(n=1,N,
  a(n)*X^n/n!
);

\\ A(x) に代入する級数
P=-log(1-2*X)/2;

\\ 関数方程式の左辺
L=subst(A,x,P);

\\ 関数方程式の右辺
R=3*A-2*X;

print("left  = ",L);
print("right = ",R);
print("equal = ",L==R);