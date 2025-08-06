\\ a(n) = Sum_{k=0..n} 3^k * (-2)^(n-k) * (n-k+1) * binomial(2*n+2,k).
a(n) = sum(k=0, n, 3^k * (-2)^(n-k) * (n-k+1) * binomial(2*n+2, k));
for(n=0, 20, print1(a(n), ", "));



