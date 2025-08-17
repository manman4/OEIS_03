\\ a(n) = Sum_{k=0..n} (-3)^(n-k) * binomial(5*n+1,k).
a(n) = sum(k=0, n, (-3)^(n-k) * binomial(5*n+1, k));
for(n=0, 20, print1(a(n), ", "));
