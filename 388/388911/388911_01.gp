M=17;

\\ a(n) = Sum_{k=0..n} 2^k * binomial(n,k) * binomial(2*n+2*k,k).
a(n) = sum(k=0, n, 2^k * binomial(n,k)*binomial(2*n+2*k,k));
for(n=0, M, print1(a(n), ", "))

