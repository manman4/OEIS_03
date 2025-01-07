M=19;

\\ a(n) = -2 * n! * Sum_{k=0..n} (-k-2)^(n-k-1) * binomial(2*k+1,k)/(n-k)!.
a(n) = -2 * n! * sum(k=0, n, (-k-2)^(n-k-1) * binomial(2*k+1,k)/(n-k)!);
for(n=0, M, print1(a(n),", "));

