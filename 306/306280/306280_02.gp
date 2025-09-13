\\ a(n) = Sum_{k=0..n} 2^(n-k) * binomial(n,k) * binomial(n^2,k).
a(n) = sum(k=0, n, 2^(n-k) * binomial(n,k) * binomial(n^2,k));
for(n=0, 20, print1(a(n),", "));


   