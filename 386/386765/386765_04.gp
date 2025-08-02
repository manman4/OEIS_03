\\ a(n) = Sum_{k=0..n} 5^k * (-2)^(n-k) * binomial(4*n,k).
a(n) = sum(k=0, n, 5^k * (-2)^(n-k) * binomial(4*n,k));
for(n=0, 20, print1(a(n), ", "));



