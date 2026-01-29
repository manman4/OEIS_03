\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} binomial(2*n-2*k,n) * binomial(4*n-3*k+1,k).
a(n) = (1/(n+1)) * sum(k=0, n\2, binomial(2*n-2*k,n) * binomial(4*n-3*k+1,k));
for(n=0, 24, print1(a(n), ", "));

