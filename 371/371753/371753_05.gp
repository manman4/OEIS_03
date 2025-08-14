\\ a(n) = Sum_{k=0..n} (-1)^k * 2^(n-k) * binomial(5*n+1,k) * binomial(5*n-k,n-k).
a(n) = sum(k=0, n, (-1)^k * 2^(n-k) * binomial(5*n+1,k) * binomial(5*n-k,n-k));
for(n=0, 20, print1(a(n),", "));