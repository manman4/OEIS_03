M=18;

\\ a(n) = 3 * n! * Sum_{k=0..n} k^(n-k) * binomial(3*n-2*k+3,k)/( (3*n-2*k+3)*(n-k)! ).
a(n) = 3* n! * sum(k=0, n, k^(n-k) * binomial(3*n-2*k+3,k)/((3*n-2*k+3)*(n-k)!));
for(n=0, M, print1(a(n), ", ")) 