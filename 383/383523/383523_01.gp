M=34;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n,k) * binomial(n-3*k,k).
a(n) = sum(k=0, n\4, binomial(n,k) * binomial(n-3*k,k));
for(n=0, M, print1(a(n),", "))         