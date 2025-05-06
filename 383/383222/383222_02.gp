M=19;

\\ a(n) = Sum_{k=4..n} 2^(k-4) * 3^(n-k) * binomial(k,4) * |Stirling1(n,k)|.
a(n) = sum(k=4, n, 2^(k-4) * 3^(n-k) * binomial(k, 4) * abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = Sum_{k=4..n} (3*n-1)^(k-4) * 3^(n-k) * binomial(k,4) * Stirling1(n,k).
b(n) = sum(k=4, n, (3*n-1)^(k-4) * 3^(n-k) * binomial(k, 4) * stirling(n, k, 1));
for(n=0, 100, print1(a(n)-b(n), ", "));


