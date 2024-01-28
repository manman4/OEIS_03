M=26;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} binomial(n+1,k) * binomial(2*n-4*k,n-3*k).
a(n) = (1/(n+1)) * sum(k=0, n\3, binomial(n+1,k)*binomial(2*n-4*k,n-3*k));
for(n=0, M, print1(a(n),", "))  

