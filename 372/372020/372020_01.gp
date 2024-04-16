\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 16^k * binomial(n/4+1/4,k) * binomial(n-1,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 16^k * binomial(n/4+1/4,k) * binomial(n-1,n-k));
for(n=0, 23, print1(a(n), ", "))
