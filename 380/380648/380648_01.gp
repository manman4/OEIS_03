M=15;

\\ a(n) = 4 * n! * Sum_{k=0..n} (4*n+4)^(k-1) * binomial(4*n+4,n-k)/k!.
a(n) = 4 * n! * sum(k=0, n, (4*n+4)^(k-1) * binomial(4*n+4, n-k)/k!);
for(n=0, M, print1(a(n),", "));
