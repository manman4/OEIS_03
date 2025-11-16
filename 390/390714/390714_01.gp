M=22;
\\ a(0) = 1; a(n) = 5 * Sum_{k=0..n} k * binomial(4*n+k,n-k)/(4*n+k).
a(n) = if(n==0, 1, 5 * sum(k=0, n, k * binomial(4*n+k, n-k)/(4*n+k)));
for(n=0, M, print1(a(n),", "))  