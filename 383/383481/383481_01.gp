M=27;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n+k-1,k) * binomial(2*n-3*k-1,n-4*k).
b(n) = sum(k=0, n\4, binomial(n+k-1, k) * binomial(2*n-3*k-1, n-4*k));
for(n=0, M, print1(b(n), ", "))

a(n, s=4, t=1, u=0) = sum(k=0, n\s, binomial(t*n+k-1, k)*binomial((t-u+1)*n-(s-1)*k-1, n-s*k));
for(n=0, M, print1(a(n)-b(n), ", "))

