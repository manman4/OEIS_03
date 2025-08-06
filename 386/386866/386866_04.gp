\\ a(n) = Sum_{k=0..n} 3^k * (-2)^(n-k) * binomial(3*n+2,k) * binomial(n-k+2,n-k).
a(n) = sum(k=0, n, 3^k * (-2)^(n-k) * binomial(3*n+2, k) * binomial(n-k+2, n-k));
for(n=0, 20, print1(a(n), ", "));



