M=17;

\\ a(n) = Sum_{k=0..n} k * (2*n+1)^(k-1) * |Stirling1(n,k)|.
a(n) = sum(k=0, n, k * (2*n+1)^(k-1) * abs(stirling(n,k,1)));
for(n=1, M, print1(a(n),", "));
