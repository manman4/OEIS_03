M=29;

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(n,k) * binomial(2*k,n-2*k).
a(n) = sum(k=0, n\2, binomial(n,k)*binomial(2*k,n-2*k));
for(n=0, M, print1(a(n), ", "))

