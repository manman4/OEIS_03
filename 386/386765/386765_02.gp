\\ a(n) = Sum_{k=0..n} 3^k * 2^(n-k) * binomial(4*n,k) * binomial(4*n-k-1,n-k).
a(n) = sum(k=0, n, 3^k * 2^(n-k) * binomial(4*n,k) * binomial(4*n-k-1,n-k));
for(n=0, 20, print1(a(n), ", "));



