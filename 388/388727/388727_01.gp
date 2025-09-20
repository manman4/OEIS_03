M=19;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(2*n+4*k,k).
a(n) = sum(k=0, n, binomial(n,k)*binomial(2*n+4*k,k));
for(n=0, M, print1(a(n), ", "))

