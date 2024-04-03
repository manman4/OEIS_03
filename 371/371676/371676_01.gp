M=18;

a(n, r=2, t=4, u=1) = r*sum(k=0, n, binomial(n, k)*binomial(t*n+u*k+r, n)/(t*n+u*k+r));
for(n=0, M, print1(a(n), ", "))

\\ a(n) = 2 * Sum_{k=0..n} binomial(n,k) * binomial(4*n+k+2,n)/(4*n+k+2).
b(n) = 2 * sum(k=0, n, binomial(n, k)*binomial(4*n+k+2, n)/(4*n+k+2));
for(n=0, 40, print1(a(n)-b(n),", ")) 


