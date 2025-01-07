M=17;

\\ a(n) = 2 * n! * Sum_{k=0..n} (n+2)^(k-1) * binomial(n+2,n-k)/k!.
a(n) = 2 * n! * sum(k=0, n, (n+2)^(k-1) * binomial(n+2,n-k)/k!);
for(n=0, M, print1(a(n),", "));

