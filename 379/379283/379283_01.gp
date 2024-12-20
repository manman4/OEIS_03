M=18;

\\ a(n) = 2 * Sum_{k=0..n} binomial(2*n+5*k+2,k) * binomial(3*n+3*k+1,n-k)/(2*n+5*k+2).
a(n) = 2 * sum(k=0, n, binomial(2*n+5*k+2, k) * binomial(3*n+3*k+1, n-k)/(2*n+5*k+2));
for(n=0, M, print1(a(n),", "))

