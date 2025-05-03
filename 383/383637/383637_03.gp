M=24;
\\ a(n) = Sum_{k=0..floor(n/2)} 16^k * binomial(n+2,2*k+2).
a(n) = sum(k=0, n\2, 16^k * binomial(n+2, 2*k+2));
for(n=0, M, print1(a(n), ", "));
