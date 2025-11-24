\\ a(n) = Sum_{k=0..n} (-1)^k * 5^(n-k) * binomial(2*k,k).
a(n) = sum(k=0, n, (-1)^k * 5^(n-k) * binomial(2*k, k));
for(n=0, 24, print1(a(n),", ")); 