\\ a(n) = Sum_{k=0..n} 2^k * (-1)^(n-k) * binomial(n,k) * binomial(2*n+k+1,n).
a(n) = sum(k=0, n, 2^k * (-1)^(n-k) * binomial(n,k) * binomial(2*n+k+1,n));
for(n=0, 21, print1(a(n),", "));


