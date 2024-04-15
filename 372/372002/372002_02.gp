\\ a(n) = Sum_{k=0..n} 4^k * binomial(n/2-k/2+1/2,k) * binomial(k,n-k)/(n-k+1).
a(n) = sum(k=0, n, 4^k * binomial(n/2-k/2+1/2, k) * binomial(k, n-k)/(n-k+1));
for(n=0, 27, print1(a(n), ", "))
