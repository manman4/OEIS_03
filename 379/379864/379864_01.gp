M=15;

\\ a(n) = 2 * n! * Sum_{k=0..n} (3*n-k+2)^(k-1) * binomial(3*n-k+2,n-k)/k!.
a(n) = 2 * n! * sum(k=0, n, (3*n-k+2)^(k-1) * binomial(3*n-k+2,n-k)/k!);
for(n=0, M, print1(a(n),", "));

