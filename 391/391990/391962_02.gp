\\ a(n) = Sum_{k=0..floor(n/2)} (k+1) * binomial(n,k) * binomial(n-k,k).
a(n) = sum(k=0, n\2, (k+1) * binomial(n,k) * binomial(n-k,k));
for(n=0, 28, print1(a(n), ", "));



