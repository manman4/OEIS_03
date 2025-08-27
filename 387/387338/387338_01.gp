\\ a(n) = Sum_{k=0..n} 2^k * binomial(n+3,k) * binomial(n+3,k+3).
a(n) = sum(k=0, n, 2^k * binomial(n+3,k) * binomial(n+3,k+3));
for(n=0, 21, print1(a(n),", "));
