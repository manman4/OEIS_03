\\ a(n) = Sum_{k=0..n} (-9)^k * binomial(-1/3,k) * binomial(n+8*k+2,n-k).
a(n) = sum(k=0, n, (-9)^k * binomial(-1/3, k) * binomial(n+8*k+2, n-k));
for(n=0, 19, print1(a(n), ", "));



