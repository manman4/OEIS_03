M=18;

\\ a(n) = Sum_{k=1..n} k * 2^(k-1) * 5^(n-k) * |Stirling1(n,k)|.
a(n) = sum(k=1, n, k * 2^(k-1) * 5^(n-k) * abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));



