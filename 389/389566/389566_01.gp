M=21;
\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * (k+2) * binomial(4*n-7*k+2,n-2*k)/(4*n-7*k+2).
a(n) = sum(k=0, n\2, (-1)^k * (k+2) * binomial(4*n-7*k+2, n-2*k)/(4*n-7*k+2));
for(n=0, M, print1(a(n),", "));

