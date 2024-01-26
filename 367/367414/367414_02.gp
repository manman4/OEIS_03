M=26;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} binomial(n+k,k) * binomial(2*n-k,n-4*k).
b(n) = (1/(n+1)) * sum(k=0, n\4, binomial(n+k,k)*binomial(2*n-k,n-4*k));
for(n=0, M, print1(b(n),", "))  

a(n) = sum(k=0, n\4, binomial(n+k, k)*binomial(2*n-k, n-4*k))/(n+1);
for(n=0, M, print1(a(n)-b(n),", "))
