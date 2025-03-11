M=21;

\\ a(n) = Sum_{k=0..n} binomial(4*k+1,k) * binomial(n+k+1,n-k)/(4*k+1).
a(n) = sum(k=0, n, binomial(4*k+1, k) * binomial(n+k+1, n-k) / (4*k+1));
for(n=0, M, print1(a(n), ", "));

\\ Partial sums of A364592.
a364592(n) = sum(k=0, n, binomial(n+k, 2*k)*binomial(4*k, k)/(3*k+1));
for(n=0, 100, print1(a(n)-sum(k=0, n, a364592(k)), ", "));


