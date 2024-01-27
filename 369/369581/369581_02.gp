M=36;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} binomial(n+1,k) * binomial(n-3*k+1,n-4*k).
a(n) = (1/(n+1)) * sum(k=0, n\4, binomial(n+1,k)*binomial(n-3*k+1,n-4*k));
for(n=0, M, print1(a(n),", "))  
