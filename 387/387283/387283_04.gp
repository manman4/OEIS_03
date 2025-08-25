M=21;

\\ a(n) = Sum_{k=0..n} (-4)^k * 3^(n-k) * binomial(-5/2,k) * binomial(n+4,n-k).
a(n) = sum(k=0, n, (-4)^k * 3^(n-k) * binomial(-5/2,k) * binomial(n+4,n-k));
for(n=0, M, print1(a(n), ", "));