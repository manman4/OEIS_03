\\ a(n) = Sum_{k=0..n} 9^k * binomial(n/3-k/3+1/3,k) * binomial(n+k-1,n-k)/(n-k+1).
a(n) = sum(k=0, n, 9^k * binomial(n/3-k/3+1/3,k) * binomial(n+k-1,n-k)/(n-k+1));
for(n=0, 21, print1(a(n), ", "))
