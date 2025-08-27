\\ a(n) = Sum_{k=0..n} 2^k * binomial(n+2,k) * binomial(n+2,k+2).
a(n) = sum(k=0, n, 2^k * binomial(n+2,k) * binomial(n+2,k+2));
for(n=0, 22, print1(a(n),", "))
