\\ a(n) = Sum_{k=0..n} 5^k * binomial(n,k) * binomial(n+1,k).
a(n) = sum(k=0, n, 5^k * binomial(n, k) * binomial(n+1, k));
for(n=0, 20, print1(a(n), ", "));


