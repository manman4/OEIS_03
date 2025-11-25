\\ a(n) = (1/(2*n+1)) * Sum_{k=0..n} k^3 * (2*k+1) * binomial(3*n-k,n-k).
a(n) = sum(k=0, n, k^3 * (2*k+1) * binomial(3*n-k, n-k))/(2*n+1);
for(n=0, 22, print1(a(n),", ")); 