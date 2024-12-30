M=19;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..n} (n-2*k-1)^k * binomial(n+1,n-k)/k!.
a(n) = (n!/(n+1)) * sum(k=0, n, (n-2*k-1)^k * binomial(n+1, n-k)/k!);
for(n=0, M, print1(a(n),", "));

