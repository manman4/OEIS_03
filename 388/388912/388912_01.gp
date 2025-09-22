M=20;

\\ a(n) = Sum_{k=0..n} 2^(n-k) * binomial(n,k) * binomial(2*n+2*k,k).
a(n) = sum(k=0, n, 2^(n-k) * binomial(n,k)*binomial(2*n+2*k,k));
for(n=0, M, print1(a(n), ", "))

