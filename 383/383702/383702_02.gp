M=18;

\\ a(n) = Sum_{k=2..n} 3^(k-2) * 4^(n-k) * binomial(k,2) * |Stirling1(n,k)|.
a(n) = sum(k=2, n, 3^(k-2) * 4^(n-k) * binomial(k, 2) * abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = Sum_{k=2..n} (4*n-1)^(k-2) * 4^(n-k) * binomial(k,2) * Stirling1(n,k).
b(n) = sum(k=2, n, (4*n-1)^(k-2) * 4^(n-k) * binomial(k, 2) * stirling(n, k, 1));
for(n=0, 100, print1(a(n)-b(n), ", "));


