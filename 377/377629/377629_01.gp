M=16;

\\ a(n) = 4 * n! * Sum_{k=0..n} k^(n-k) * binomial(4*n+k+4,k)/( (4*n+k+4)*(n-k)! ).
a(n) = 4 * n! * sum(k=0, n, k^(n-k) * binomial(4*n+k+4,k) / (4*n+k+4) / (n-k)!);
for(n=0, M, print1(a(n), ", ")) 