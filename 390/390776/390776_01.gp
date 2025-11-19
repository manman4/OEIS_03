M=21;
\\ a(n) = (1/(4*n+2)) * Sum_{k=0..n} (4*k+2) * binomial(4*n+2,n-k).
a(n) = sum(k=0, n, (4*k+2) * binomial(4*n+2, n-k)/(4*n+2));
for(n=0, M, print1(a(n),", "));  