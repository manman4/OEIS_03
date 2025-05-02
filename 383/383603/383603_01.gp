M=27;

\\ a(n) = Sum_{k=0..floor(n/2)} (-9)^k * binomial(-1/3,k) * binomial(n-k,k).
a(n) = sum(k=0, n/2, (-9)^k * binomial(-1/3,k) * binomial(n-k,k));
for(n=0, M, print1(a(n),", "))