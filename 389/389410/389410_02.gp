\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} binomial(n+1,k) * binomial(2*n,n-2*k).
a(n) = sum(k=0, n\2, binomial(n+1, k)*binomial(2*n,n-2*k))/(n+1);
for(n=0, 25, print1(a(n), ", "));