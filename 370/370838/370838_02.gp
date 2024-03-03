M=32;

\\ a(n) = Sum_{k=0..floor(n/4)} (-1)^k * binomial(n,4*k) * binomial(5*k,k)/(4*k+1).
a(n) = sum(k=0, n\4, (-1)^k * binomial(n,4*k) * binomial(5*k,k)/(4*k+1));
for(n=0, M, print1(a(n), ", "))