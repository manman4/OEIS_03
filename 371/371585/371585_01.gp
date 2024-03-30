M=20;

a(n, r=2, s=2, t=5, u=0) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+r));
for(n=0, M, print1(a(n),", "))  

\\ a(n) = 2 * Sum_{k=0..n} binomial(5*k+2,k) * binomial(n+k-1,n-k)/(5*k+2).
b(n) = 2 * sum(k=0, n, binomial(5*k+2, k) * binomial(n+k-1, n-k)/(5*k+2));
for(n=0, 20, print1(a(n)-b(n), ", "))

