a(n, s=1, t=4, u=-4) = sum(k=0, n\s, binomial(t*(n+1)+k-1, k)*binomial((t+u+1)*(n+1)-(s-1)*k-2, n-s*k))/(n+1);
for(n=0, 20, print1(a(n), ", "))

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(4*n+k+3,k) * binomial(n-1,n-k).
b(n) = (1/(n+1)) * sum(k=0, n, binomial(4*n+k+3,k)*binomial(n-1,n-k));
for(n=0, 20, print1(a(n)-b(n), ", "))
