\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} binomial(2*n-3*k,n) * binomial(6*n-8*k+2,k).
a(n) = (1/(n+1)) * sum(k=0, n\3, binomial(2*n-3*k,n) * binomial(6*n-8*k+2,k));
for(n=0, 25, print1(a(n), ", "));

