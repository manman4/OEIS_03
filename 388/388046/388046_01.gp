\\ a(n) = Sum_{k=0..n} 2^k * binomial(n,k) * binomial(4*n+1,k).
a(n) = sum(k=0, n, 2^k * binomial(n, k) * binomial(4*n+1, k));
for(n=0, 17, print1(a(n), ", "));


