\\ a(n) = (1/(n+1)) * Sum_{k=0..n} (k+1)^5 * binomial(2*n-k,n-k).
a(n) = sum(k=0, n, (k+1)^5 * binomial(2*n-k, n-k))/(n+1);
for(n=0, 23, print1(a(n),", ")); 