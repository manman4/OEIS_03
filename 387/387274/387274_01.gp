\\ a(n) = Sum_{k=0..n} 2^(n-k) * binomial(n+4,k+4) * binomial(2*k+8,k+8).
a(n) = sum(k=0, n, 2^(n-k) * binomial(n+4,k+4) * binomial(2*k+8,k+8));
for(n=0, 20, print1(a(n),", "))
