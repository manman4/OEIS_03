M=20;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(2*n+2,k) * binomial(4*n-2*k+4,n-k).
b(n) = (1/(n+1)) * sum(k=0, n, binomial(2*n+2,k)*binomial(4*n-2*k+4,n-k));
for(n=0, M, print1(b(n),", "))  

a(n) = sum(k=0, n, binomial(2*n+2, k)*binomial(4*n-2*k+4, n-k))/(n+1);
for(n=0, M, print1(a(n)-b(n),", "))
