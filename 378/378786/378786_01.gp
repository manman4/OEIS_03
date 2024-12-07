M=21;

a(n, r=1, s=2, t=4, u=0) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(s*k, n-k)/(t*k+u*(n-k)+r));
for(n=0, M, print1(a(n),", "))                 

\\ a(n) = Sum_{k=0..n} binomial(4*k+1,k) * binomial(2*k,n-k)/(4*k+1).
b(n) = sum(k=0, n, binomial(4*k+1,k)*binomial(2*k,n-k)/(4*k+1));
for(n=0, 100, print1(a(n)-b(n),", "))

a002293(n) = binomial(4*n, n)/(3*n + 1);
\\ a(n) = Sum_{k=0..n} binomial(2*k,n-k) * A002293(k).
c(n) = sum(k=0, n, binomial(2*k,n-k)*a002293(k));
for(n=0, 100, print1(a(n)-c(n),", "))
