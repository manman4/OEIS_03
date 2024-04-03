M=17;

a(n, r=3, t=3, u=3) = r*sum(k=0, n, binomial(n, k)*binomial(t*n+u*k+r, n)/(t*n+u*k+r));
for(n=0, M, print1(a(n), ", "))

\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(3*n+3*k+3,n)/(n+k+1).
b(n) = sum(k=0, n, binomial(n, k)*binomial(3*n+3*k+3, n)/(n+k+1));
for(n=0, 40, print1(a(n)-b(n),", ")) 


