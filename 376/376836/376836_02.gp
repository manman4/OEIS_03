\\ a(n) = Sum_{k=0..floor(n/8)} (-16)^k * binomial(-1/8,k) * binomial(n,n-8*k).
a(n) = sum(k=0, n\8, binomial(-1/8, k) * binomial(n, n-8*k) * (-16)^k);
for(n=0, 35, print1(a(n),", "))



