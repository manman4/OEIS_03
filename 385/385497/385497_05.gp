\\ a(n) = Sum_{k=0..n} 2^k * binomial(6*n-k,n-k).
a(n) = sum(k=0, n, 2^k * binomial(6*n-k, n-k));
for(n=0, 25, print1(a(n), ", "));
