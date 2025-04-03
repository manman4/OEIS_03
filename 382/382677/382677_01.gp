M=25;

\\ A(n,k) = Sum_{j=0..min(n,k)} (j!)^2 * binomial(j+3,3) * Stirling2(n+1,j+1) * Stirling2(k+1,j+1).
a(n, k) = sum(j=0, min(n,k), (j!)^2 * binomial(j+3,3) * stirling(n+1,j+1,2) * stirling(k+1,j+1,2));

\\ a(n) = 9 - 28 * 2^n + 20 * 3^n.
b(n) = 9 - 28 * 2^n + 20 * 3^n;
for(n=0, M, print1(b(n), ", ")); 

\\ Column k=2 of A382674.
for(n=0, 100, print1(a(n, 2)-b(n), ", "));




