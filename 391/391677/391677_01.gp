\\ a(n) = Sum_{k=0..n} (k+1) * 2^k * 3^(n-k) * binomial(k,n-k)^2.
a(n) = sum(k=0, n, (k+1) * 2^k * 3^(n-k) * binomial(k, n-k)^2);
for(n=0, 22, print1(a(n), ", "));



