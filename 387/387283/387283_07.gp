\\ a(n) = (-1)^n * Sum_{k=0..n} 10^k * (21/10)^(n-k) * binomial(-5/2,k) * binomial(k,n-k).
a(n) = (-1)^n * sum(k=0, n, 10^k * (21/10)^(n-k) * binomial(-5/2,k) * binomial(k,n-k));
for(n=0, 20, print1(a(n), ", "));