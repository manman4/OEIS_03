a(n, s=2, t=4, u=0) = sum(k=0, n\s, binomial(t*(n+1)+k-1, k)*binomial((t+u+1)*(n+1)-(s-1)*k-2, n-s*k))/(n+1);
for(n=0, 20, print1(a(n), ", "))

\\ b(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} binomial(4*n+k+3,k) * binomial(5*n-k+3,n-2*k).
b(n) = 1/(n+1) * sum(k=0, n\2, binomial(4*n+k+3, k)*binomial(5*n-k+3, n-2*k));
for(n=0, 100, print1(a(n) - b(n), ", "))