\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+2,2) * binomial(k,n-2*k)^2.
a(n) = sum(k=0, n\2, binomial(k+2, 2) * binomial(k, n-2*k)^2);
for(n=0, 35, print1(a(n), ", "))
