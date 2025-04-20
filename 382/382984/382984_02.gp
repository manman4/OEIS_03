M=19;

\\ a(n) = Sum_{k=3..n} 3^(n-k) * binomial(k,3) * |Stirling1(n,k)|.
a(n) = sum(k=3, n, 3^(n-k) * binomial(k, 3) * abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));



