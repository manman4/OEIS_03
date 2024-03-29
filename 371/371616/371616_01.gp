M=22;

a(n, r=1, s=3, t=0, u=3) = r*sum(k=0, n, binomial(t*k+r*u*(n-k)+r, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+r*u*(n-k)+r));
for(n=0, M, print1(a(n),", "))  

\\ a(n) = Sum_{k=0..n} binomial(3*(n-k)+1,k) * binomial(n+2*k-1,n-k)/(3*(n-k)+1).
b(n) = sum(k=0, n, binomial(3*(n-k)+1,k)*binomial(n+2*k-1,n-k)/(3*(n-k)+1));
for(n=0, 26, print1(a(n)-b(n), ", "))

