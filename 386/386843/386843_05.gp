M=25;

\\ a(n) = Sum_{k=0..n} 2^k * (n-k+1) * binomial(n+k,k).
a(n) = sum(k=0, n, 2^k * (n-k+1) * binomial(n+k, k));
for(n=0, M, print1(a(n), ", "))

