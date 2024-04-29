a(n, s=3, t=3, u=0) = sum(k=0, n\s, binomial(t*(n+1), k)*binomial((t+u)*(n+1)-k, n-s*k))/(n+1);
for(n=0, 22, print1(a(n), ", "))

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} binomial(3*n+3,k) * binomial(3*n-k+3,n-3*k).
b(n) = sum(k=0, n\3, binomial(3*n+3,k) * binomial(3*n-k+3,n-3*k))/(n+1);
for(n=0, 40, print1(a(n)-b(n), ", "))

