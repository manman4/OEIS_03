\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 2^k * binomial(n+1,k) * binomial(4*n-2*k+4,n-k).
a(n) = sum(k=0, n, 2^k * binomial(n+1, k)*binomial(4*n-2*k+4, n-k))/(n+1);
for(n=0, 20, print1(a(n), ", "));