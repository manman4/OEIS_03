\\ a(n) = Sum_{k=0..floor(n/2)} (-9)^k * binomial(-1/3,k) * binomial(n+k,n-2*k).
a(n) = sum(k=0, n\2, (-9)^k * binomial(-1/3, k) * binomial(n+k, n-2*k));
for(n=0, 25, print1(a(n),", "))



