\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 25^k * binomial(n/5+1/5,k) * binomial(k,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 25^k * binomial(n/5+1/5, k) * binomial(k, n-k));
for(n=0, 21, print1(a(n), ", "))

\\ a(5*n+4) = 0 for n >= 0.
for(n=0, 500, print1(a(5*n+4), ", "))
