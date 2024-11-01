M=18;

\\ a(n) = 2 * n! * Sum_{k=0..n} k^(n-k) * binomial(4*n-3*k+2,k)/( (4*n-3*k+2)*(n-k)! ).
a(n) = 2 * n! * sum(k=0, n, k^(n-k) * binomial(4*n-3*k+2,k)/((4*n-3*k+2)*(n-k)!));
for(n=0, M, print1(a(n), ", ")) 