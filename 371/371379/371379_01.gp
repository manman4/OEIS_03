M=19;

\\ a(n) = 4 * Sum_{k=0..n} binomial(n-1,n-k) * binomial(6*k+3,k)/(5*k+4).
a(n) = 4 * sum(k=0, n, binomial(n-1, n-k) * binomial(6*k+3, k)/(5*k+4));
for(n=0, M, print1(a(n),", "))  
