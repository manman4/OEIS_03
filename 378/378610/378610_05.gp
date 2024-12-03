\\ a(n) = 4 * Sum_{k=0..n} 2^k * (-1)^(n-k) * binomial(n,k) * binomial(4*n+k+4,n)/(4*n+k+4).
a(n) = 4 * sum(k=0, n, 2^k * (-1)^(n-k) * binomial(n,k) * binomial(4*n+k+4,n)/(4*n+k+4));
for(n=0, 20, print1(a(n), ", "))
