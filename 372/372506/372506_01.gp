M=20;

\\ a(n) = Sum_{k=0..n} binomial(n+k-1,k) * binomial(3*n-1,n-k).
b(n) = sum(k=0, n, binomial(n+k-1, k) * binomial(3*n-1, n-k));
for(n=0, M, print1(b(n), ", "))

a(n, s=1, t=1, u=1) = sum(k=0, n\s, binomial(t*n+k-1, k)*binomial((t+u+1)*n-(s-1)*k-1, n-s*k));
for(n=0, M, print1(a(n)-b(n), ", "))

