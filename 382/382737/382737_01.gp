M=16;

\\ A(n,k) = Sum_{j=0..min(n,k)} j! * (j+1)! * Stirling2(n,j) * Stirling2(k,j).
a(n, k) = sum(j=0, min(n, k), j!*(j+1)!*stirling(n, j, 2)*stirling(k, j, 2));

\\ a(n) = Sum_{k=0..n} k! * (k+1)! * Stirling2(n,k)^2.
b(n) = sum(k=0, n, k!*(k+1)!*stirling(n, k, 2)^2);
for(n=0, M, print1(b(n), ", "));

\\ a(n) == 0 (mod 2) for n > 0.
for(n=0, 100, print1(b(n)%2, ", "));

\\ Main diagonal of A382734.
for(n=0, 100, print1(a(n, n)-b(n), ", "));




