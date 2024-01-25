M=18;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(2*n+2,k) * binomial(6*n-3*k+6,n-k).
b(n) = (1/(n+1)) * sum(k=0, n, binomial(2*n+2,k)*binomial(6*n-3*k+6,n-k));
for(n=0, M, print1(b(n),", "))  

a(n) = sum(k=0, n, binomial(2*n+2, k)*binomial(6*n-3*k+6, n-k))/(n+1);
for(n=0, M, print1(a(n)-b(n),", "))
