\\ a(n) = Sum_{k=0..n} (-4)^k * binomial(-7/2,k) * binomial(k,n-k).
a(n) = sum(k=0, n, (-4)^k*binomial(-7/2, k)*binomial(k, n-k));
for(n=0, 20, print1(a(n), ", ")) 