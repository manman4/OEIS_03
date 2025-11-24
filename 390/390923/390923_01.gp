\\ a(n) = Sum_{k=0..n} 2^k * 3^(n-k) * binomial(2*k,k).
a(n) = sum(k=0, n, 2^k * 3^(n-k) * binomial(2*k, k));
for(n=0, 21, print1(a(n),", ")); 