\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 4^k * binomial(n/2+k-1/2,k) * binomial(k,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 4^k * binomial(n/2+k-1/2,k) * binomial(k,n-k));
for(n=0, 20, print1(a(n), ", "))

