\\ a(n) = Sum_{k=0..floor(n/2)} 2^(n-k) * binomial(n-k,k)^2.
a(n) = sum(k=0, n\2, 2^(n-k) * binomial(n-k, k)^2);
for(n=0, 25, print1(a(n), ", "))

