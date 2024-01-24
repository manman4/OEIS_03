M=22;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} binomial(n+1,k) * binomial(3*n-k+3,n-3*k).
b(n) = (1/(n+1)) * sum(k=0, n\3, binomial(n+1,k)*binomial(3*n-k+3,n-3*k));
for(n=0, M, print1(b(n),", "))  

a(n, s=3, t=1, u=2) = sum(k=0, n\s, binomial(t*(n+1), k)*binomial((t+u)*(n+1)-k, n-s*k))/(n+1);
for(n=0, M, print1(a(n)-b(n),", "))
