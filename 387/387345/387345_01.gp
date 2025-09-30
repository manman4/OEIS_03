M=35;

a(n, s=4, t=1, u=1) = sum(k=0, n\s, binomial(t*n, k)*binomial((u-t+1)*n-(s-1)*k-1, n-s*k));
for(n=0, M, print1(a(n), ", "))

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n,k) * binomial(n-3*k-1,n-4*k).
b(n) = sum(k=0, n\4, binomial(n,k)*binomial(n-3*k-1,n-4*k));
for(n=0, 100, print1(a(n)-b(n), ", "))

