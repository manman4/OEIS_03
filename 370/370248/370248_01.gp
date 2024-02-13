M=25;

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*n,k) * binomial(2*n-3*k-1,n-3*k).
b(n) = sum(k=0, n\3, binomial(2*n, k)*binomial(2*n-3*k-1, n-3*k));
for(n=0, M, print1(b(n), ", "))

a(n, s=3, t=2, u=1) = sum(k=0, n\s, binomial(t*n, k)*binomial((u+1)*n-s*k-1, n-s*k));
for(n=0, M, print1(a(n)-b(n), ", "))

