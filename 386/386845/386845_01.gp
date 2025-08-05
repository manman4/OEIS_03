M=18;

\\ a(n) = Sum_{k=0..n} binomial(4*n+2,k) * binomial(4*n-k,n-k).
a(n) = sum(k=0, n, binomial(4*n+2,k) * binomial(4*n-k,n-k));
for(n=0, M, print1(a(n), ", "))

