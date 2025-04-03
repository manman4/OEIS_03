M=9;

\\ A(n,k) = Sum_{j=0..min(n,k)} (j!)^2 * binomial(j+3,3) * Stirling2(n+1,j+1) * Stirling2(k+1,j+1).
a(n, k) = sum(j=0, min(n,k), (j!)^2 * binomial(j+3,3) * stirling(n+1,j+1,2) * stirling(k+1,j+1,2));
for(n=0, M, for(k=0, n, print1(a(k, n-k)", ")));






