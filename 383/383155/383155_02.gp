\\ a(n) = Sum_{k=0..n} k^6 * (-1)^k * 3^(n-k) * binomial(n,k).
a(n) = sum(k=0, n, k^6 * (-1)^k * 3^(n-k) * binomial(n,k));
for(n=0, 20, print1(a(n), " ,"));

