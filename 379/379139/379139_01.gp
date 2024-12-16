M=22;

\\ a(n) = 3 * Sum_{k=0..n} binomial(n+k+3,k) * binomial(n+k+3,n-k)/(n+k+3).
a(n) = 3 * sum(k=0, n, binomial(n+k+3, k) * binomial(n+k+3, n-k)/(n+k+3));  
for(n=0, M, print1(a(n),", "))

