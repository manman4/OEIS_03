M=20;

\\ a(n) = Sum_{k=2..n} 3^(n-k) * binomial(k,2) * Stirling2(n,k).
a(n) = sum(k=2, n, binomial(k, 2) * 3^(n-k) * stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));



