\\ a(n) = Sum_{k=0..n} 3^k * (-2)^(n-k) * binomial(5*n,k) * binomial(5*n-k-1,n-k).
a(n) = sum(k=0, n, 3^k * (-2)^(n-k) * binomial(5*n,k) * binomial(5*n-k-1,n-k));
for(n=0, 23, print1(a(n), ", "));


