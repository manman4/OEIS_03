M=31;

\\ a(n) = Sum_{k=0..floor(n/3)} (-1)^k * binomial(n,3*k) * binomial(4*k,k)/(3*k+1).
a(n) = sum(k=0, n\3, (-1)^k * binomial(n,3*k) * binomial(4*k,k)/(3*k+1));
for(n=0, M, print1(a(n), ", "))