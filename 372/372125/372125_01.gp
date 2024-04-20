\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 4^(n-k) * binomial(n+k,k) * binomial(k/2,n-k).
a(n) = 1/(n+1) * sum(k=0, n, 4^(n-k) * binomial(n+k,k) * binomial(k/2,n-k));
for(n=0, 24, print1(a(n), ", "))

