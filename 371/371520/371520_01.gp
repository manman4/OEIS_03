M=20;

\\ a(n) = 5 * Sum_{k=0..n} binomial(n-1,n-k) * binomial(5*k+4,k)/(4*k+5).
a(n) = 5 * sum(k=0, n, binomial(n-1, n-k) * binomial(5*k+4, k)/(4*k+5));
for(n=0, M, print1(a(n),", "))  
