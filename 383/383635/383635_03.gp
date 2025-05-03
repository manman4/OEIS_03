M=24;

\\ a(n) = Sum_{k=0..n} 5^k * binomial(n+4,k+4) * Stirling2(k+4,4).
a(n) = sum(k=0, n, 5^k * binomial(n+4, k+4) * stirling(k+4, 4, 2));
for(n=0, M, print1(a(n), ", "));
