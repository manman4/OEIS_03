M=17;

\\ a(n) = 2 * Sum_{k=0..n} binomial(5*n+2*k+2,k) * binomial(5*n+2,n-k)/(5*n+2*k+2).
a(n) = 2 * sum(k=0, n, binomial(5*n+2*k+2,k) * binomial(5*n+2,n-k)/(5*n+2*k+2));
for(n=0, M, print1(a(n), ", "))




 