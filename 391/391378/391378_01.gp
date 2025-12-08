M=21;
\\ a(n) = Sum_{k=0..n} (-1)^k * (k+2) * binomial(4*n-3*k+2,n-k)/(4*n-3*k+2).
a(n) = sum(k=0, n, (-1)^k * (k+2) * binomial(4*n-3*k+2,n-k)/(4*n-3*k+2));
for(n=0, M, print1(a(n),", "));

