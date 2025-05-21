M=17;

\\ a(n) = Sum_{k=0..n} k * (n+4)^(k-1) * 4^(n-k) * |Stirling1(n,k)|.
a(n) = sum(k=0, n, k * (n+4)^(k-1) * 4^(n-k) * abs(stirling(n,k,1)));
for(n=1, M, print1(a(n),", "));
