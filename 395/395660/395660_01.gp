\\ a(n) = Sum_{k=0..n} 4^k * binomial(k+4,4) * binomial(5*n+5,n-k).
a(n) = sum(k=0, n, 4^k * binomial(k+4,4) * binomial(5*n+5,n-k));
for(n=0, 20, print1(a(n), ", "));

