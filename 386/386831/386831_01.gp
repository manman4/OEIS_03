M=16;

\\ a(n) = Sum_{k=0..n} 3^k * 2^(n-k) * binomial(4*n+1,k) * binomial(4*n-k,n-k).
a(n) = sum(k=0, n, 3^k * 2^(n-k) * binomial(4*n+1,k) * binomial(4*n-k,n-k));
for(n=0, M, print1(a(n), ", "))

