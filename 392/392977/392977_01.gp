\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} binomial(2*n-2*k,n) * binomial(6*n-5*k+2,k).
a(n) = (1/(n+1)) * sum(k=0, n\2, binomial(2*n-2*k,n) * binomial(6*n-5*k+2,k));
for(n=0, 23, print1(a(n), ", "));

