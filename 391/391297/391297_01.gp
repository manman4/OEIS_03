\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * (2*k+2) * binomial(3*n-4*k+2,n-2*k)/(3*n-4*k+2).
a(n) = sum(k=0, n\2, (-1)^k * (2*k+2) * binomial(3*n-4*k+2, n-2*k)/(3*n-4*k+2) );
for(n=0, 23, print1(a(n), ", "));



