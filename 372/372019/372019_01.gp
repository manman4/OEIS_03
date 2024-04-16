\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 9^k * binomial(n/3+1/3,k) * binomial(n-1,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 9^k * binomial(n/3+1/3,k) * binomial(n-1,n-k));
for(n=0, 27, print1(a(n), ", "))
