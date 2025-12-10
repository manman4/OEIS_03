M=21;
\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * (k+1) * binomial(4*n-6*k+2,n-2*k)/(2*n-3*k+1).
a(n) = sum(k=0, n\2, (-1)^k * (k+1) * binomial(4*n-6*k+2, n-2*k)/(2*n-3*k+1));
for(n=0, M, print1(a(n),", "));

