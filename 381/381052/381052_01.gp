M=22;

\\ a(n) = Sum_{k=2..n} 3^(n-k) * binomial(n,k) * |Stirling1(k,2)|.
a(n) = sum(k=2, n, 3^(n-k) * binomial(n, k) * abs(stirling(k, 2, 1)));
for(n=0, M, print1(a(n), ", "));



