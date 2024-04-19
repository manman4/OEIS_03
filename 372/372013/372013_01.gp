\\ a(n) = (-1)^n * Sum_{k=0..n} 9^(n-k) * binomial(n,k) * binomial(k/3,n-k)/(n-k+1).
a(n) = (-1)^n * sum(k=0, n, 9^(n-k) * binomial(n,k) * binomial(k/3,n-k)/(n-k+1));
for(n=0, 22, print1(a(n), ", "))

