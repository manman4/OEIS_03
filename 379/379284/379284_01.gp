M=20;

\\ a(n) = Sum_{k=0..n} binomial(2*n+3*k+1,k) * binomial(3*n+k,n-k)/(2*n+3*k+1).
a(n) = sum(k=0, n, binomial(2*n+3*k+1, k) * binomial(3*n+k, n-k)/(2*n+3*k+1));
for(n=0, M, print1(a(n),", "))

