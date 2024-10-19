\\ a(n) = Sum_{k=0..n} (-4)^k * binomial(-5/2,k) * binomial(k,n-k).
a(n) = sum(k=0, n, (-4)^k*binomial(-5/2, k)*binomial(k, n-k));
for(n=0, 22, print1(a(n), ", ")) 