M=41;

\\ a(n) = Sum_{k=1..n} phi(k) * binomial(floor(n/k)+3,4).
a(n) = sum(k=1, n, eulerphi(k) * binomial(n\k+3, 4));
for(n=1, M, print1(a(n), ", "));
