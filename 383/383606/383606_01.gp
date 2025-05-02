M=31;

\\ a(n) = Sum_{k=0..floor(n/3)} (-9)^k * binomial(-2/3,k) * binomial(n-2*k,k).
a(n) = sum(k=0, n/3, (-9)^k * binomial(-2/3,k) * binomial(n-2*k,k));
for(n=0, M, print1(a(n),", "))