\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+2,2) * binomial(n,k) * binomial(n-k,k).
a(n) = sum(k=0, n\2, binomial(k+2,2) * binomial(n,k) * binomial(n-k,k));
for(n=0, 26, print1(a(n), ", "));



