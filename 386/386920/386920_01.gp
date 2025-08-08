M=19;

\\ a(n) = Sum_{k=0..n} binomial(4*n,k) * binomial(3*n-k,n-k).
a(n) = sum(k=0, n, binomial(4*n, k) * binomial(3*n-k, n-k));
for(n=0, M, print1(a(n)", "));