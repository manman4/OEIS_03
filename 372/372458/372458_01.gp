M=20;

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(2*n+k-1,k) * binomial(4*n-k-1,n-2*k).
b(n) = sum(k=0, n\2, binomial(2*n+k-1, k) * binomial(4*n-k-1, n-2*k));
for(n=0, M, print1(b(n), ", "))

a(n, s=2, t=2, u=1) = sum(k=0, n\s, binomial(t*n+k-1, k)*binomial((t+u+1)*n-(s-1)*k-1, n-s*k));
for(n=0, M, print1(a(n)-b(n), ", "))

