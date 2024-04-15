\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 16^k * binomial(n/4+1/4,k) * binomial(k,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 16^k * binomial(n/4+1/4, k) * binomial(k, n-k));
for(n=0, 25, print1(a(n), ", "))

\\ a(4*n+3) = 0 for n >= 0.
for(n=0, 500, print1(a(4*n+3), ", "))
