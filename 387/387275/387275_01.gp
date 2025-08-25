\\ a(n) = Sum_{k=0..n} 3^(n-k) * binomial(n+2,k+2) * binomial(2*k+4,k+4).
a(n) = sum(k=0, n, 3^(n-k) * binomial(n+2,k+2) * binomial(2*k+4,k+4));
for(n=0, 20, print1(a(n),", "))
