M=21;
\\ a(n) = Sum_{k=0..floor(n/3)} (-1)^k * (k+1) * binomial(4*n-10*k+2,n-3*k)/(2*n-5*k+1).
a(n) = sum(k=0, n\3, (-1)^k * (k+1) * binomial(4*n-10*k+2, n-3*k)/(2*n-5*k+1));
for(n=0, M, print1(a(n),", "));

