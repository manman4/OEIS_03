M=29;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n,k) * binomial(n+k,n-4*k).
a(n) = sum(k=0, n\4, binomial(n,k)*binomial(n+k,n-4*k));
for(n=0, M, print1(a(n), ", "))

