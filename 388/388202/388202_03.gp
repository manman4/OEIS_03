\\ a(n) = Sum_{k=0..n} 3^k * (-2)^(n-k) * binomial(n,k) * binomial(n+k+2,k).
a(n) = sum(k=0, n, 3^k * (-2)^(n-k) * binomial(n, k) * binomial(n+k+2, k));
for(n=0, 19, print1(a(n),", "));


