M=19;

\\ a(n) = -3 * n! * Sum_{k=0..n} (-2*n-3)^(n-k-1) * binomial(2*n+k+2,k)/(n-k)!.
a(n) = -3 * n! * sum(k=0, n, (-2*n-3)^(n-k-1) * binomial(2*n+k+2,k)/(n-k)!);
for(n=0, M, print1(a(n),", "));

