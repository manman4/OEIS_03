M=17;

\\ a(n) = Sum_{k=0..n} binomial(3*n+4*k+2,k) * binomial(3*n+3*k+3,n-k)/(n+k+1).
a(n) = sum(k=0, n, binomial(3*n+4*k+2, k) * binomial(3*n+3*k+3, n-k)/(n+k+1));
for(n=0, M, print1(a(n),", "))

