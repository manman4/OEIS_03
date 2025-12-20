\\ a(n) = Sum_{k=0..n} 2^k * 3^(n-k) * binomial(k+6,6) * binomial(2*k,2*(n-k)).
a(n) = sum(k=0, n, 2^k * 3^(n-k) * binomial(k+6,6) * binomial(2*k,2*(n-k)));
for(n=0, 20, print1(a(n), ", "));



