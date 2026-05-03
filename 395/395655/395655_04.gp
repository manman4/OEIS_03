\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 2^k * binomial(k+2,2) * binomial(3*n+3,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 2^k * binomial(k+2,2) * binomial(3*n+3,n-k));
for(n=0, 20, print1(a(n), ", "));
