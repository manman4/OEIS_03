M=17;

\\ a(n) = Sum_{k=0..n} k * (n+3)^(k-1) * 3^(n-k) * |Stirling1(n,k)|.
a(n) = sum(k=0, n, k * (n+3)^(k-1) * 3^(n-k) * abs(stirling(n,k,1)));
for(n=1, M, print1(a(n),", "));
