M=19;

\\ a(n) = -n! * Sum_{k=0..n} (-3*n+k-1)^(n-k-1) * binomial(3*n,k)/(n-k)!.
a(n) = -n! * sum(k=0, n, (-3*n+k-1)^(n-k-1) * binomial(3*n,k)/(n-k)!);
for(n=0, M, print1(a(n),", "));

