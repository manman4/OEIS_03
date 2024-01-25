M=19;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} binomial(2*n+2,k) * binomial(6*n-3*k+6,n-3*k).
b(n) = (1/(n+1)) * sum(k=0, n\3, binomial(2*n+2,k)*binomial(6*n-3*k+6,n-3*k));
for(n=0, M, print1(b(n),", "))  

a(n) = sum(k=0, n\3, binomial(2*n+2, k)*binomial(6*n-3*k+6, n-3*k))/(n+1);
for(n=0, M, print1(a(n)-b(n),", "))
