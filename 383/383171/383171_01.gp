M=19;

\\ a(n) = Sum_{k=2..n} 2^(n-k) * |Stirling1(n,k) * Stirling1(k,2)|.
a(n) = sum(k=2, n, 2^(n-k) * abs(stirling(n,k,1) * stirling(k,2,1)));
for(n=0, M, print1(a(n), ", "));
