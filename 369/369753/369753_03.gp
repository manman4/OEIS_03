a000587(n) = sum(k=0, n, (-1)^k * stirling(n, k, 2));
\\ a(n) = Sum_{k=0..n} 5^k * Stirling1(n,k) * A000587(k).
a(n) = sum(k=0, n, 5^k * stirling(n, k, 1) * a000587(k));
for(n=0, 20, print1(a(n), ", "));