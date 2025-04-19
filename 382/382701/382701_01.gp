M=22;

\\ a(n) = Sum_{k=3..n} 3^(n-k) * binomial(n,k) * |Stirling1(k,3)|.
a(n) = sum(k=3, n, 3^(n-k) * binomial(n, k) * abs(stirling(k, 3, 1)));
for(n=0, M, print1(a(n), ", "));



