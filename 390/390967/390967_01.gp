\\ a(n) = (1/(n+1)) * Sum_{k=0..n} k^4 * (k+1) * binomial(2*n-k,n-k).
a(n) = sum(k=0, n, k^4 * (k+1) * binomial(2*n-k, n-k))/(n+1);
for(n=0, 24, print1(a(n),", ")); 