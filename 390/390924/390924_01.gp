\\ a(n) = Sum_{k=0..n} 3^k * 2^(n-k) * binomial(2*k,k).
a(n) = sum(k=0, n, 3^k * 2^(n-k) * binomial(2*k, k));
for(n=0, 20, print1(a(n),", ")); 