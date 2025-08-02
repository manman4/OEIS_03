M=16;

\\ a(n) = Sum_{k=0..n} 5^k * 3^(n-k) * binomial(3*n+k-1,k).
a(n) = sum(k=0, n, 5^k * 3^(n-k) * binomial(3*n+k-1,k));
for(n=0, M, print1(a(n), ", "))

