\\ a(n) = 3 * Sum_{k=0..n} 2^k * (-1)^(n-k) * binomial(n,k) * binomial(3*n+k+3,n)/(3*n+k+3).
a(n) = 3 * sum(k=0, n, 2^k * (-1)^(n-k) * binomial(n,k) * binomial(3*n+k+3,n)/(3*n+k+3));
for(n=0, 20, print1(a(n), ", "))
