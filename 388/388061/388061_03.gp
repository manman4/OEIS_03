\\ a(n) = Sum_{k=0..n} 5^k * (-4)^(n-k) * binomial(n,k) * binomial(n+k+1,k).
a(n) = sum(k=0, n, 5^k * (-4)^(n-k) * binomial(n, k) * binomial(n+k+1, k));
for(n=0, 19, print1(a(n),", "));


