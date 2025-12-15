a(n, s=4, t=1, u=2) = sum(k=0, n, binomial(t*k+u*(n-k)+1, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+1));
for(n=0, 22, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} binomial(2*n-k+1,k) * binomial(n+3*k-1,n-k)/(2*n-k+1).
b(n) = sum(k=0, n, binomial(2*n-k+1, k)*binomial(n+3*k-1, n-k)/(2*n-k+1));
for(n=0, 100, print1(a(n)-b(n), ", "));



