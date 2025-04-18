M=20;

\\ a(n) = Sum_{k=1..n} 2^(n-k) * |Stirling1(n,k) * Stirling1(k,1)|.
a(n) = sum(k=1, n, 2^(n-k) * abs(stirling(n,k,1) * stirling(k,1,1)));
for(n=0, M, print1(a(n), ", "));
