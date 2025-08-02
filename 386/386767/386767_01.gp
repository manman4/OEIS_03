M=17;

\\ a(n) = Sum_{k=0..n} 5^k * 2^(n-k) * binomial(2*n+k-1,k).
a(n) = sum(k=0, n, 5^k * 2^(n-k) * binomial(2*n+k-1,k));
for(n=0, M, print1(a(n), ", "))

