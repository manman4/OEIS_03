M=21;

\\ a(n) = Sum_{k=0..floor(n/3)} (-9)^(n-3*k) * binomial(-1/3,n-3*k) * binomial(n-2*k,k).
a(n) = sum(k=0, n/3, (-9)^(n-3*k) * binomial(-1/3,n-3*k) * binomial(n-2*k,k));
for(n=0, M, print1(a(n),", "))