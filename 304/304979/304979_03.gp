M=24;

\\ a(n) = (1/(5*n+1)) * Sum_{k=0..n} (5*k+1) * binomial(5*n+1,n-k).
a(n) = (1/(5*n+1)) * sum(k=0, n, (5*k+1) * binomial(5*n+1, n-k));
for(n=0, M, print1(a(n),", "));  

