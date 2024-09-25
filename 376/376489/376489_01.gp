M=30;

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(3*k,k) * binomial(n-k-1,n-2*k) / (2*k+1).
a(n) = sum(k=0, n\2, binomial(3*k, k) * binomial(n-k-1, n-2*k) / (2*k+1));
for(n=0, M, print1(a(n), ", "));