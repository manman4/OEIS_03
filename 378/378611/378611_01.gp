M=21;

\\ a(n) = Sum_{k=0..n} binomial(2*n+k-1,k) * binomial(n-1,n-k).
a(n) = sum(k=0, n, binomial(2*n+k-1, k) * binomial(n-1, n-k));
for(n=0, M, print1(a(n), ", "));

a259554(n) = sum(i=0, n, 2^i*(-1)^(i+n)*binomial(n, i)*binomial(2*n+i-1, n-1));
\\ a(n) = 2 * A259554(n) for n > 0.
for(n=1, 100, print1(a(n)-2*a259554(n), ", "));