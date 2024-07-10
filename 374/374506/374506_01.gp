\\ a(n) = (binomial(n+6,3)/20) * Sum_{k=0..floor(n/2)} binomial(n+3,n-2*k) * binomial(2*k+3,k).
a(n) = binomial(n+6, 3)/20 * sum(k=0, n\2, binomial(n+3, n-2*k)*binomial(2*k+3, k));
for(n=0, 24, print1(a(n), ", ")) 