M=30;

\\ a(n) = Sum_{k=0..floor(n/2)} (2*k+1) * binomial(2*k,k) * binomial(n-k-1,n-2*k).
a(n) = sum(k=0, n\2, (2*k+1)*binomial(2*k, k)*binomial(n-k-1, n-2*k));
for(n=0, M, print1(a(n), ", ")) 