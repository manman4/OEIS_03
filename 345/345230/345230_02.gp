M=26;

\\ a(n) = Sum_{k=1..n} phi(k) * binomial(floor(n/k)+n-1,n).
a(n) = sum(k=1, n, eulerphi(k)*binomial(n\k+n-1, n));
for(n=0, M, print1(a(n), ", "));