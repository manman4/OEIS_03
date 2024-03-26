M=20;

\\ a(n) = 2 * Sum_{k=0..n} binomial(n-1,n-k) * binomial(4*k+1,k)/(3*k+2).
a(n) = 2 * sum(k=0, n, binomial(n-1, n-k) * binomial(4*k+1, k)/(3*k+2));
for(n=0, M, print1(a(n),", "))  
