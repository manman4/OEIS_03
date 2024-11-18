a(n, r=4, t=2, u=2) = r*sum(k=0, n, binomial(n, k)*binomial(t*n+u*k+r, n)/(t*n+u*k+r));
for(n=0, 20, print1(a(n), ", "))

\\ a(n) = 2 * Sum_{k=0..n} binomial(n,k) * binomial(2*n+2*k+4,n)/(n+k+2).
b(n) = 2 * sum(k=0, n, binomial(n, k)*binomial(2*n+2*k+4, n)/(n+k+2));
for(n=0, 40, print1(a(n)-b(n),", ")) 


