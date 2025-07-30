\\ a(n) = Sum_{k=0..n} (-3)^(n-k) * binomial(4*n,k).
a(n) = sum(k=0, n, (-3)^(n-k) * binomial(4*n, k));
for(n=0, 21, print1(a(n), ", "));


