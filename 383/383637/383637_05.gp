M=24;
\\ a(n) = Sum_{k=0..n} 4^k * (-3)^(n-k) * binomial(n+2,k+2) * Stirling2(k+2,2).
a(n) = sum(k=0, n, 4^k * (-3)^(n-k) * binomial(n+2,k+2) * stirling(k+2,2,2));
for(n=0, M, print1(a(n), ", "));
