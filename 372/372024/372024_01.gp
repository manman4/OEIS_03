\\ a(n) = Sum_{k=0..n} 9^k * binomial(1/3,k) * binomial(n-1,n-k).
a(n) = sum(k=0, n, 9^k * binomial(n-1,n-k) * binomial(1/3,k));
for(n=0, 22, print1(a(n), ", "))
