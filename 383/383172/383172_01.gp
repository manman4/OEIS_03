M=19;

\\ a(n) = Sum_{k=3..n} 2^(n-k) * |Stirling1(n,k) * Stirling1(k,3)|.
a(n) = sum(k=3, n, 2^(n-k) * abs(stirling(n,k,1) * stirling(k,3,1)));
for(n=0, M, print1(a(n), ", "));
