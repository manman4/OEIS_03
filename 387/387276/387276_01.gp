\\ a(n) = Sum_{k=0..n} 3^(n-k) * binomial(n+3,k+3) * binomial(2*k+6,k+6).
a(n) = sum(k=0, n, 3^(n-k) * binomial(n+3,k+3) * binomial(2*k+6,k+6));
for(n=0, 20, print1(a(n),", "))
