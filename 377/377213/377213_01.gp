M=35;

\\ a(n) = Sum_{k=0..floor(n/2)} (2*k+1) * binomial(2*k,k) * binomial(n-2*k-1,n-3*k).
a(n) = sum(k=0, n\2, (2*k+1)*binomial(2*k, k)*binomial(n-2*k-1, n-3*k));
for(n=0, M, print1(a(n), ", ")) 