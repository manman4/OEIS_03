M=20;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(n+1,k) * binomial(4*n-4*k+2,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, binomial(n+1,k)*binomial(4*n-4*k+2,n-k));
for(n=0, M, print1(a(n),", "))  

