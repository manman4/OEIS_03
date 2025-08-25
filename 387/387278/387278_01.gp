\\ a(n) = Sum_{k=0..n} 3^(n-k) * binomial(n+1,k+1) * binomial(2*k+2,k+2).
a(n) = sum(k=0, n, 3^(n-k) * binomial(n+1,k+1) * binomial(2*k+2,k+2));
for(n=0, 21, print1(a(n),", "))
