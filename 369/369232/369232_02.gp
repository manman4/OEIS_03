M=30;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} binomial(3*n+3,k) * binomial(n-2*k-1,n-3*k).
b(n) = sum(k=0, n\3, binomial(3*n+3,k) * binomial(n-2*k-1,n-3*k) )/(n+1);
for(n=0, M, print1(b(n), ", "))

a(n, s=3, t=3, u=3) = sum(k=0, n\s, binomial(t*(n+1), k)*binomial((u-t+1)*(n+1)-(s-1)*k-2, n-s*k))/(n+1);
for(n=0, M, print1(a(n)-b(n), ", "))
