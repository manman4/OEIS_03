a(n, s=3, t=1, u=4) = sum(k=0, n, binomial(t*k+u*(n-k)+1, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+1));
for(n=0, 22, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} binomial(4*n-3*k+1,k) * binomial(n+2*k-1,n-k)/(4*n-3*k+1).
b(n) = sum(k=0, n, binomial(4*n-3*k+1, k)*binomial(n+2*k-1, n-k)/(4*n-3*k+1));
for(n=0, 100, print1(a(n)-b(n), ", "));



