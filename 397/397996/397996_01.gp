a_vector(n) = my(v=vector(n)); v[1]=1; for(i=1, n-1, v[i+1]=sum(j=1, i, 2^(i-j)*stirling(i+1, j, 2)*v[j])); v;
a_vector(30)

\\ E.g.f. A(x) satisfies A((exp(2*x) - 1)/2) = 3*A(x) - 2*x.
N=30;
v=a_vector(N);
X=x+O(x^(N+1));
A=sum(n=1, N, v[n]*X^n/n!);
P=(exp(2*X) - 1)/2;
if(subst(A,x,P)==3*A-2*X, print("ok"), print("not ok"));


\\ a(1) = 1; a(n+1) = Sum_{k=1..n} 2^(n-k) * Stirling2(n+1,k) * a(k).
\\ a(1) = 1; a(n) = Sum_{k=1..n-1} 2^(n-1-k) * Stirling2(n,k) * a(k).
a(n) = if(n==1, 1, sum(k=1, n-1, 2^(n-1-k) * stirling(n, k, 2) * a(k)));
for(n=1, 15, print1(a(n),", "));

