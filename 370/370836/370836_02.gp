M=31;

\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * binomial(n,2*k) * binomial(3*k,k)/(2*k+1).
a(n) = sum(k=0, n\2, (-1)^k * binomial(n,2*k) * binomial(3*k,k)/(2*k+1));
for(n=0, M, print1(a(n), ", "))