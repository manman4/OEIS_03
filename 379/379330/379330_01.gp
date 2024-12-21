M=22;

\\ a(n) = 2^n * Sum_{k=0..n} binomial(n/2+3*k/2+1/2,k) * binomial(3*n/2-k/2-1/2,n-k)/(n+3*k+1).
a(n) = 2^n * sum(k=0, n, binomial(n/2+3*k/2+1/2,k) * binomial(3*n/2-k/2-1/2,n-k)/(n+3*k+1));
for(n=0, M, print1(a(n),", "))

