M=26;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n+k,k) * binomial(2*n-3*k,n-4*k).
a(n) = sum(k=0, n\4, binomial(n+k, k) * binomial(2*n-3*k, n-4*k));
for(n=0, M, print1(a(n), ", "))



