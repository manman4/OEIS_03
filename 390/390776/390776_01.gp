M=21;
\\ a(n) = (1/(2*n+1)) * Sum_{k=0..n} (2*k+1) * binomial(4*n+2,n-k).
a(n) = sum(k=0, n, (2*k+1) * binomial(4*n+2, n-k)/(2*n+1));
for(n=0, M, print1(a(n),", "));  