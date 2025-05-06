M=18;

\\ a(n) = Sum_{k=3..n} 4^(n-k) * binomial(k,3) * |Stirling1(n,k)|.
a(n) = sum(k=3, n, 4^(n-k) * binomial(k, 3) * abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = Sum_{k=3..n} (4*n-3)^(k-3) * 4^(n-k) * binomial(k,3) * Stirling1(n,k).
b(n) = sum(k=3, n, (4*n-3)^(k-3) * 4^(n-k) * binomial(k, 3) * stirling(n, k, 1));
for(n=0, 100, print1(a(n)-b(n), ", "));


