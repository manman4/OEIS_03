\\ a(n) = Sum_{k=0..n} 2^(n-k) * binomial(n,k) * binomial(2*n+k,n).
a(n) = sum(k=0, n, 2^(n-k) * binomial(n,k) * binomial(2*n+k,n));
for(n=0, 17, print1(a(n),", "));


