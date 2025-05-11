\\ a(n) = (1/2^n) * Sum_{k=0..n} (n-2*k)^(2*n) * binomial(n,k).
a(n) = (1/2^n) * sum(k=0, n, (n-2*k)^(2*n) * binomial(n, k));
for(n=0, 15, print1(a(n), ", "));
