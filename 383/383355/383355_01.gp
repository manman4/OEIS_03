M=40;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*k,k) * binomial(n-3*k,k).
a(n) = sum(k=0, n\4, binomial(2*k,k) * binomial(n-3*k,k));
for(n=0, M, print1(a(n),", "))