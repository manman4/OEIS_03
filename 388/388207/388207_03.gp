\\ a(n) = Sum_{k=0..n} 4^k * (-3)^(n-k) * binomial(n,k) * binomial(n+k+4,k).
a(n) = sum(k=0, n, 4^k * (-3)^(n-k) * binomial(n, k) * binomial(n+k+4, k));
for(n=0, 19, print1(a(n),", "));


