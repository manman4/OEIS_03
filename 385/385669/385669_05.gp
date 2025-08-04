M=25;

\\ a(n) = Sum_{k=0..n} 5^k * 2^(n-k) * binomial(n+k,k).
a(n) = sum(k=0, n, 5^k * 2^(n-k) * binomial(n+k, k));
for(n=0, M, print1(a(n), ", "))

