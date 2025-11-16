M=21;
\\ a(n) = Sum_{k=0..n} (-3)^k * binomial(3*n+k,n-k).
a(n) = sum(k=0, n, (-3)^k * binomial(3*n+k, n-k));
for(n=0, M, print1(a(n),", "));