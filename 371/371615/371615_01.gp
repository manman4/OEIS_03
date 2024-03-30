M=21;

a(n, r=2, s=2, t=0, u=6) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+r));
for(n=0, M, print1(a(n),", "))  

\\ a(n) = Sum_{k=0..n} binomial(6*(n-k)+2,k) * binomial(n+k-1,n-k)/(3*(n-k)+1).
b(n) = sum(k=0, n, binomial(6*(n-k)+2,k)*binomial(n+k-1,n-k)/(3*(n-k)+1));
for(n=0, 26, print1(a(n)-b(n), ", "))

