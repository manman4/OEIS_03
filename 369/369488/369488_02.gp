M=23;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} binomial(3*n+k+2,k) * binomial(2*n-k,n-2*k).
b(n) = (1/(n+1)) * sum(k=0, n\2, binomial(3*n+k+2,k)*binomial(2*n-k,n-2*k));
for(n=0, M, print1(b(n),", "))  

a(n, s=2, t=3, u=2) = sum(k=0, n\s, binomial(t*(n+1)+k-1, k)*binomial((t-u+1)*(n+1)-(s-1)*k-2, n-s*k))/(n+1);
for(n=0, M, print1(a(n)-b(n),", "))
