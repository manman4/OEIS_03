M=21;
\\ a(0) = 1; a(n) = 3 * Sum_{k=0..n} k * binomial(5*n-2*k,n-k)/(5*n-2*k).
a(n) = if(n==0, 1, 3 * sum(k=0, n, k * binomial(5*n-2*k, n-k)/(5*n-2*k)));
for(n=0, M, print1(a(n),", "))  