M=19;

\\ a(n) = Sum_{k=0..n} binomial(2*n+4*k+1,k) * binomial(3*n+2*k,n-k)/(2*n+4*k+1).
a(n) = sum(k=0, n, binomial(2*n+4*k+1, k) * binomial(3*n+2*k, n-k)/(2*n+4*k+1));
for(n=0, M, print1(a(n),", "))

