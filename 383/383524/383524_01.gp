M=26;

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(n,k) * binomial(n-k,k)^2.
a(n) = sum(k=0, n\2, binomial(n,k) * binomial(n-k,k)^2);
for(n=0, M, print1(a(n),", "))         