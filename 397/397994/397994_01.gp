a_vector(n) = my(v=vector(n)); v[1]=1; for(i=1, n-1, v[i+1]=sum(j=1, i, 3^(i-j)*abs(stirling(i+1, j, 1))*v[j])); v;
a_vector(18)

\\ E.g.f. A(x) satisfies A(-log(1-3*x)/3) = 4*A(x) - 3*x.
N=30;
v=a_vector(N);
X=x+O(x^(N+1));
A=sum(n=1, N, v[n]*X^n/n!);
P=-log(1-3*X)/3;
if(subst(A, x, P)==4*A-3*X, print("ok"), print("not ok"));


\\ a(1) = 1; a(n+1) = Sum_{k=1..n} 3^(n-k) * |Stirling1(n+1,k)| * a(k).
\\ a(1) = 1; a(n) = Sum_{k=1..n-1} 3^(n-1-k) * |Stirling1(n,k)| * a(k).
a(n) = if(n==1, 1, sum(k=1, n-1, 3^(n-1-k) * abs(stirling(n, k, 1)) * a(k)));
for(n=1, 15, print1(a(n),", "));

