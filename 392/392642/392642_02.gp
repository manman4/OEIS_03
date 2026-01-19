\\ a(n) = Sum_{k=0..floor(n/2)} binomial(2*k,k) * binomial(n+4*k+2,n-2*k).
a(n) = sum(k=0, n\2, binomial(2*k, k) * binomial(n+4*k+2, n-2*k));
for(n=0, 25, print1(a(n), ", "));



