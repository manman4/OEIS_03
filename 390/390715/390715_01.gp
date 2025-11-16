M=22;
\\ a(0) = 1; a(n) = 2 * Sum_{k=0..n} k * binomial(5*n-3*k,n-k)/(5*n-3*k).
a(n) = if(n==0, 1, 2 * sum(k=0, n, k * binomial(5*n-3*k, n-k)/(5*n-3*k)));
for(n=0, M, print1(a(n),", "))  