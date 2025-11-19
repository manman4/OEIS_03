M=21;
\\ a(n) = (1/(4*n+3)) * Sum_{k=0..n} (4*k+3) * binomial(4*n+3,n-k).
a(n) = sum(k=0, n, (4*k+3) * binomial(4*n+3, n-k)/(4*n+3));
for(n=0, M, print1(a(n),", "));  