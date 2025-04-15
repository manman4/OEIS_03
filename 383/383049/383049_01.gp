M=10;

\\ A(n,k) = Sum_{j=0..n} (j+1)^k * Stirling1(n,j).
a(n, k) = sum(j=0, n, (j+1)^k*stirling(n, j, 1));
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));
