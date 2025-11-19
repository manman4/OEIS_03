M=20;
\\ a(n) = (1/(5*n+3)) * Sum_{k=0..n} (5*k+3) * binomial(5*n+3,n-k).
a(n) = sum(k=0, n, (5*k+3) * binomial(5*n+3, n-k)/(5*n+3));
for(n=0, M, print1(a(n),", "));  