\\ a(n) = Sum_{k=0..n} n^k * (-4*n+1)^(n-k) * binomial(n,k) * binomial(2*k,k).
a(n) = sum(k=0, n, n^k * (-4*n+1)^(n-k) * binomial(n,k) * binomial(2*k,k));
for(n=0, 20, print1(a(n), ", "))
