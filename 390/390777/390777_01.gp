M=20;
\\ a(n) = (1/(5*n+2)) * Sum_{k=0..n} (5*k+2) * binomial(5*n+2,n-k).
a(n) = sum(k=0, n, (5*k+2) * binomial(5*n+2, n-k)/(5*n+2));
for(n=0, M, print1(a(n),", "));  