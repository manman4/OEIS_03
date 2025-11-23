M=19;

\\ a(n) = Sum_{k=0..n} 2^k * 5^(n-k) * Stirling2(n,k).
a(n) = sum(k=0, n, 2^k * 5^(n-k) * stirling(n, k, 2) );
for(n=0, M, print1(a(n),", "));



