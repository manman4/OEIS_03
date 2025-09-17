\\ a(n) = Sum_{k=0..n} 2^(n-k) * binomial(n,k) * binomial(n+k+3,k).
a(n) = sum(k=0, n, 2^(n-k) * binomial(n, k) * binomial(n+k+3, k));
for(n=0, 25, print1(a(n),", "));


