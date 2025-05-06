M=16;

\\ a(n) = Sum_{k=0..n} (-(4*n-3))^k * 4^(n-k) * binomial(n+k,n) * |Stirling1(2*n,n+k)|.
a(n) = sum(k=0, n, (-4*n+3)^k * 4^(n-k) * binomial(n+k, n) * abs(stirling(2*n, n+k, 1)));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} (4*n-1)^k * 4^(n-k) * binomial(n+k,n) * Stirling1(2*n,n+k).
b(n) = sum(k=0, n, (4*n-1)^k * 4^(n-k) * binomial(n+k, n) * stirling(2*n, n+k, 1));
for(n=0, 100, print1(a(n)-b(n), ", "));


