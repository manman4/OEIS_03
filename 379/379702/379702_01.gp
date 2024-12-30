M=18;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..n} (2*n-3*k-1)^k * binomial(n+1,n-k)/k!.
a(n) = (n!/(n+1)) * sum(k=0, n, (2*n-3*k-1)^k * binomial(n+1, n-k)/k!);
for(n=0, M, print1(a(n),", "));

