M=19;

\\ a(n) = 6 * Sum_{k=0..n} binomial(n-1,n-k) * binomial(6*k+5,k)/(5*k+6).
a(n) = 6 * sum(k=0, n, binomial(n-1, n-k) * binomial(6*k+5, k)/(5*k+6));
for(n=0, M, print1(a(n),", "))  
