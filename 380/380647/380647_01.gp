M=16;

\\ a(n) = 3 * n! * Sum_{k=0..n} (3*n+3)^(k-1) * binomial(3*n+3,n-k)/k!.
a(n) = 3 * n! * sum(k=0, n, (3*n+3)^(k-1) * binomial(3*n+3, n-k)/k!);
for(n=0, M, print1(a(n),", "));
