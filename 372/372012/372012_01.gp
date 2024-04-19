\\ a(n) = (-1)^n * Sum_{k=0..n} 4^(n-k) * binomial(n,k) * binomial(k/2,n-k)/(n-k+1).
a(n) = (-1)^n * sum(k=0, n, 4^(n-k) * binomial(n,k) * binomial(k/2,n-k)/(n-k+1));
for(n=0, 28, print1(a(n), ", "))

