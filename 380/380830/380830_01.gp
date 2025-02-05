M=17;

\\ a(n) = n! * Sum_{k=0..n} (3*n+3)^k * binomial(n,k)/(k+1)!.
a(n) = n! * sum(k=0, n, (3*n+3)^k * binomial(n, k)/(k+1)!);
for(n=0, M, print1(a(n),", "));

a376094(n) = n!*sum(k=1, n, (3*n)^(k-1)*binomial(n-1, k-1)/k!);
\\ a(n) = A376094(n+1)/(n+1).
for(n=0, M, print1(a(n) - a376094(n+1)/(n+1), ", "));