\\ a(n) = Sum_{k=0..n} 3^k * binomial(n,k) * binomial(2*n,k).
a(n) = sum(k=0, n, 3^k * binomial(n, k) * binomial(2*n, k));
for(n=0, 17, print1(a(n), ", "));


