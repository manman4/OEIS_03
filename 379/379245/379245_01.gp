M=17;

\\ a(n) = Sum_{k=0..n} binomial(3*n+3*k+3,k) * binomial(4*n+2*k+2,n-k)/(n+k+1).
a(n) = sum(k=0, n, binomial(3*n+3*k+3, k) * binomial(4*n+2*k+2, n-k)/(n+k+1));
for(n=0, M, print1(a(n),", "))
