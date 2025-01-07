M=17;

\\ a(n) = 3 * n! * Sum_{k=0..n} (2*n+3)^(k-1) * binomial(2*n+3,n-k)/k!.
a(n) = 3 * n! * sum(k=0, n, (2*n+3)^(k-1) * binomial(2*n+3,n-k)/k!);
for(n=0, M, print1(a(n),", "));

\\ a(n) == 0 (mod 3) for n>0.
for(n=0, 100, print1(a(n) % 3, ", "));

