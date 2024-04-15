\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 9^k * binomial(n/3+1/3,k) * binomial(k,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 9^k * binomial(n/3+1/3, k) * binomial(k, n-k));
for(n=0, 32, print1(a(n), ", "))

\\ a(3*n+2) = 0 for n > 0.
for(n=0, 500, print1(a(3*n+2), ", "))
