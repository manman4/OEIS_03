a(n, s=2, t=4, u=-3) = sum(k=0, n\s, binomial(t*(n+1), k)*binomial((t+u)*(n+1)-k, n-s*k))/(n+1);
for(n=0, 25, print1(a(n), ", "))

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} binomial(4*n+4,k) * binomial(n-k+1,n-2*k).
b(n) = sum(k=0, n\2, binomial(4*n+4,k) * binomial(n-k+1,n-2*k))/(n+1);
for(n=0, 40, print1(a(n)-b(n), ", "))

