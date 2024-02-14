M=20;

\\ a(n) = Sum_{k=0..n} binomial(n+k-1,k) * binomial(3*n+k-1,n-k).
b(n) = sum(k=0, n, binomial(n+k-1, k)*binomial(3*n+k-1, n-k));
for(n=0, M, print1(b(n), ", "))

