\\ a(n) = Sum_{k=0..floor(n/4)} (-8)^k * binomial(-1/4,k) * binomial(n,n-4*k).
a(n) = sum(k=0, n\4, binomial(-1/4, k) * binomial(n, n-4*k) * (-8)^k);
for(n=0, 31, print1(a(n),", "))



