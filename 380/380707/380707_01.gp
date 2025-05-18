M=14;

\\ a(n) = Sum_{k=0..n} (k+1) * n^(2*k) * |Stirling1(n+1,k+1)|.
a(n) = sum(k=0, n, (k+1)*n^(2*k)*abs(stirling(n+1, k+1, 1)));
for(n=0, M, print1(a(n),", "));


