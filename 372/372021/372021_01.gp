\\ a(n) = Sum_{k=0..n} 4^k * binomial(n/2-k/2+1/2,k) * binomial(n+k-1,n-k)/(n-k+1).
a(n) = sum(k=0, n, 4^k * binomial(n/2-k/2+1/2,k) * binomial(n+k-1,n-k)/(n-k+1));
for(n=0, 26, print1(a(n), ", "))
