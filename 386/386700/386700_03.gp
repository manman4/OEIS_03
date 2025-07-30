\\ a(n) = Sum_{k=0..n} (-2)^(n-k) * binomial(2*n+k-1,k).
a(n) = sum(k=0, n, (-2)^(n-k) * binomial(2*n+k-1, k));
for(n=0, 23, print1(a(n), ", "));


