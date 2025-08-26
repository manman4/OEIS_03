\\ a(n) = Sum_{k=0..n} 3^k * binomial(n+1,k+1) * binomial(2*k+2,k+2).
a(n) = sum(k=0, n, 3^k * binomial(n+1,k+1) * binomial(2*k+2,k+2));
for(n=0, 20, print1(a(n),", "))
