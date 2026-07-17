a_vector(n) = my(v=vector(n)); v[1]=1; for(i=1, n-1, v[i+1]=sum(j=1, i, 3^(i-j)*stirling(i, j, 2)*v[j])); v;
a_vector(30)

\\ E.g.f. A(x) satisfies A'(x) = 1 + A((exp(3*x) - 1)/3), with A(0) = 0.
N=30;
v=a_vector(N);
X=x+O(x^(N+1));
A=sum(n=1, N, v[n]*X^n/n!);
P=(exp(3*X) - 1)/3;
if(deriv(A) == 1 + subst(A, x, P), print("ok"), print("not ok"));


\\ a(1) = 1; a(n+1) = Sum_{k=1..n} 3^(n-k) * Stirling2(n,k) * a(k).
\\ a(1) = 1; a(n) = Sum_{k=1..n-1} 3^(n-1-k) * Stirling2(n-1,k) * a(k).
a(n) = if(n==1, 1, sum(k=1, n-1, 3^(n-1-k) * stirling(n-1, k, 2) * a(k)));
for(n=1, 15, print1(a(n),", "));

