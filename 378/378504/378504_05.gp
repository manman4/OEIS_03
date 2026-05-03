\\ a(n) = Sum_{k=0..n} 2^k * binomial(k+2,2) * binomial(3*n+3,n-k).
a(n) = sum(k=0, n, 2^k * binomial(k+2,2) * binomial(3*n+3,n-k));
for(n=0, 20, print1(a(n), ", "));

