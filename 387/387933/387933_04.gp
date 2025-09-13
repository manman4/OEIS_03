\\ a(n) = Sum_{k=0..n} 3^k * (-2)^(n-k) * binomial(n,k) * binomial(4*n+k,n).
a(n) = sum(k=0, n, 3^k * (-2)^(n-k) * binomial(n,k) * binomial(4*n+k,n));
for(n=0, 19, print1(a(n),", "));


