M=10;

\\ A(n,k) = Sum_{j=0..min(n,k)} j! * (j+1)! * Stirling2(n,j) * Stirling2(k,j).
a(n, k) = sum(j=0, min(n, k), j! * (j+1)! * stirling(n, j, 2) * stirling(k, j, 2));
for(n=0, M, for(k=0, n, print1(a(k, n-k)", ")));






