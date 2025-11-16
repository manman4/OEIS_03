M=22;
\\ a(0) = 1; a(n) = Sum_{k=0..n} k * binomial(4*n-2*k,n-k)/(2*n-k).
a(n) = if(n==0, 1, sum(k=0, n, k * binomial(4*n-2*k, n-k)/(2*n-k)));
for(n=0, M, print1(a(n),", "))  