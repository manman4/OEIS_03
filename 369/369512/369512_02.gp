M=17;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(2*n+k+1,k) * binomial(7*n+2*k+5,n-k).
b(n) = (1/(n+1)) * sum(k=0, n, binomial(2*n+k+1,k)*binomial(7*n+2*k+5,n-k));
for(n=0, M, print1(b(n),", "))  

a(n) = sum(k=0, n, binomial(2*n+k+1, k)*binomial(7*n+2*k+5, n-k))/(n+1);
for(n=0, M, print1(a(n)-b(n),", "))
