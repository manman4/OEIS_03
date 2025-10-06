\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} binomial(n+k,k) * binomial(2*n+k+2,n-2*k).
a(n) = sum(k=0, n\2, binomial(n+k, k)*binomial(2*n+k+2, n-2*k))/(n+1);
for(n=0, 24, print1(a(n), ", "));