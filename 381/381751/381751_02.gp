M=16;

\\ a(0) = 1; a(n) = (1/n) * Sum_{k=1..n} binomial(8*k-1,2*k-1) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, binomial(8*k-1,2*k-1) * a(n-k))/n);
for(n=0, M, print1(a(n), ", "))
 