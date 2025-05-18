M=9;

\\ A(n,k) = Sum_{j=0..n} (j+1) * (k*n)^j * |Stirling1(n+1,j+1)|.
a(n, k) = sum(j=0, n, (j+1)*(k*n)^j*abs(stirling(n+1, j+1, 1)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));


