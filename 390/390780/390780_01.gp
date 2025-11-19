M=20;
\\ a(n) = (1/(5*n+4)) * Sum_{k=0..n} (5*k+4) * binomial(5*n+4,n-k).
a(n) = sum(k=0, n, (5*k+4) * binomial(5*n+4, n-k)/(5*n+4));
for(n=0, M, print1(a(n),", "))  