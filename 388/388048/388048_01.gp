\\ a(n) = Sum_{k=0..n} 3^k * binomial(n,k) * binomial(2*n+1,k).
a(n) = sum(k=0, n, 3^k * binomial(n, k) * binomial(2*n+1, k));
for(n=0, 19, print1(a(n), ", "));


