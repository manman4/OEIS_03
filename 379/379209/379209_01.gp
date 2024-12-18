M=21;

\\ a(n) = Sum_{k=0..n} binomial(n+2*k+1,k) * binomial(2*n,n-k)/(n+2*k+1).
a(n) = sum(k=0, n, binomial(n+2*k+1, k) * binomial(2*n, n-k)/(n+2*k+1));
for(n=0, M, print1(a(n),", "))

\\ a(n) = sum(k=0..n, binomial(2*n,n+k)*binomial(n+2*k,k)*(n+1)/(n+k+1)).
b(n) = sum(k=0, n, binomial(2*n, n+k) * binomial(n+2*k, k) * (n+1)/(n+k+1));
\\ a(n) = A190738(n)/(n+1).
for(n=0, 100, print1(a(n)-b(n)/(n+1),", "))