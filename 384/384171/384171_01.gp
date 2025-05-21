M=17;

\\ a(n) = Sum_{k=0..n} k * (3*n+2)^(k-1) * 2^(n-k) * |Stirling1(n,k)|.
a(n) = sum(k=0, n, k * (3*n+2)^(k-1) * 2^(n-k) * abs(stirling(n,k,1)));
for(n=1, M, print1(a(n),", "));
