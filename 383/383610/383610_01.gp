M=20;

\\ a(n) = Sum_{k=0..floor(n/2)} (-9)^(n-2*k) * binomial(-2/3,n-2*k) * binomial(n-k,k).
a(n) = sum(k=0, n/2, (-9)^(n-2*k) * binomial(-2/3,n-2*k) * binomial(n-k,k));
for(n=0, M, print1(a(n),", "))