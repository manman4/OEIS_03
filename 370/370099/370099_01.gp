M=20;

\\ a(n) = Sum_{k=0..n} binomial(2*n,k) * binomial(3*n-k-1,n-k).
a(n) = sum(k=0, n, binomial(2*n, k) * binomial(3*n-k-1, n-k));
for(n=0, M, print1(a(n), ", "))

