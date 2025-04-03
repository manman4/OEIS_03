M=15;

\\ A(n,k) = Sum_{j=0..min(n,k)} (j!)^2 * binomial(j+2,2) * Stirling2(n+1,j+1) * Stirling2(k+1,j+1).
a(n, k) = sum(j=0, min(n,k), (j!)^2 * binomial(j+2,2) * stirling(n+1,j+1,2) * stirling(k+1,j+1,2));

\\ a(n) = Sum_{k=0..n} (k!)^2 * binomial(k+2,2) * Stirling2(n+1,k+1)^2.
b(n) = sum(k=0, n, (k!)^2 * binomial(k+2,2) * stirling(n+1,k+1,2)^2);
for(n=0, M, print1(b(n), ", "));

\\ Main diagonal of A382673.
for(n=0, 100, print1(a(n, n)-b(n), ", "));




