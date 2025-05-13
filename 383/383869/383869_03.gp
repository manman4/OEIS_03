\\ a(n) = Sum_{k=0..n} n^(n-k) * binomial(2*n,n+k) * Stirling2(n+k,n).
a(n) = sum(k=0, n, n^(n-k) * binomial(2*n, n+k) * stirling(n+k, n, 2));
for(n=0, 20, print1(a(n),", "));


