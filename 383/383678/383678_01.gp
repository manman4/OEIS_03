M=17;

\\ a(n) = Sum_{k=0..n} (k+1) * (2*n)^k * |Stirling1(n+1,k+1)|.
a(n) = sum(k=0, n, (k+1)*(2*n)^k*abs(stirling(n+1, k+1, 1)));
for(n=0, M, print1(a(n),", "));


