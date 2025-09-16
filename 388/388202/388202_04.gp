\\ a(n) = Sum_{k=0..n} 2^(n-k) * binomial(n,k) * binomial(n+k+2,n).
a(n) = sum(k=0, n, 2^(n-k) * binomial(n,k) * binomial(n+k+2,n));
for(n=0, 18, print1(a(n),", "));


