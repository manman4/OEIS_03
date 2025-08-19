\\ a(n) = Sum_{k=0..n} (-1)^k * 5^(n-k) * binomial(2*k,k) * binomial(n+1,n-k).
a(n) = sum(k=0, n, (-1)^k * 5^(n-k) * binomial(2*k, k) * binomial(n+1, n-k));
for(n=0, 20, print1(a(n), ", "));
