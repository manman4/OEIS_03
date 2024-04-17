\\ a(n) = Sum_{k=0..n} 9^k * binomial(1/3,k) * binomial(k,n-k).
a(n) = sum(k=0, n, 9^k * binomial(1/3,k) * binomial(k,n-k));
for(n=0, 22, print1(a(n), ", "))

