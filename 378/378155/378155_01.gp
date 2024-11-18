a(n, r=3, t=2, u=2) = r*sum(k=0, n, binomial(n, k)*binomial(t*n+u*k+r, n)/(t*n+u*k+r));
for(n=0, 20, print1(a(n), ", "))

\\ a(n) = 3 * Sum_{k=0..n} binomial(n,k) * binomial(2*n+2*k+3,n)/(2*n+2*k+3).
b(n) = 3 * sum(k=0, n, binomial(n, k)*binomial(2*n+2*k+3, n)/(2*n+2*k+3));
for(n=0, 40, print1(a(n)-b(n),", ")) 


