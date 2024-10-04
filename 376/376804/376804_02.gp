\\ a(n) = Sum_{k=0..floor(n/3)} (-9)^k * binomial(-1/3,k) * binomial(n,n-3*k).
a(n) = sum(k=0, n\3, (-9)^k * binomial(-1/3, k) * binomial(n, n-3*k));  
for(n=0, 29, print1(a(n),", "))



