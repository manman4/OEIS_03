\\ a(n) = Sum_{k=0..n} 2^k * (-1)^(n-k) * binomial(n,k) * binomial(3*n+k+1,k).
a(n) = sum(k=0, n, 2^k * (-1)^(n-k) * binomial(n, k) * binomial(3*n+k+1, k));
for(n=0, 19, print1(a(n),", "));


