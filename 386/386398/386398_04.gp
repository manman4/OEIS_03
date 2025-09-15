\\ a(n) = Sum_{k=0..n} 5^k * (-4)^(n-k) * binomial(n,k) * binomial(n+k+1,n).
a(n) = sum(k=0, n, 5^k * (-4)^(n-k) * binomial(n,k) * binomial(n+k+1,n));
for(n=0, 18, print1(a(n),", "));


