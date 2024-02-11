M=25;

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(n,k) * binomial(2*n-k,n-3*k).
b(n) = sum(k=0, n\3, binomial(n, k) * binomial(2*n-k, n-3*k));
for(n=0, M, print1(b(n), ", "))

a(n, s=3, t=1, u=1) = sum(k=0, n\s, binomial(t*n, k)*binomial((t+u)*n-k, n-s*k));
for(n=0, M, print1(a(n)-b(n), ", "))

