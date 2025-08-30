\\ a(n) = Sum_{k=0..floor(n/2)} (n*(n+1))^k * (2*n+1)^(n-2*k) * binomial(n,2*k) * binomial(2*k,k).
a(n) = sum(k=0, n\2, (n*(n+1))^k * (2*n+1)^(n-2*k) * binomial(n,2*k) * binomial(2*k,k));
for(n=0, 20, print1(a(n),", "))
