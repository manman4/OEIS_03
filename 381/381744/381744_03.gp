M=17;

\\ a(0) = 1; a(n) = (1/n) * Sum_{k=1..n} binomial(6*k-1,2*k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, binomial(6*k-1,2*k) * a(n-k))/n);
for(n=0, M, print1(a(n), ", "))
 