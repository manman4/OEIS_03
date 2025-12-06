\\ a(n) = Sum_{k=0..floor(n/4)} (-1)^k * (4*k+2) * binomial(3*n-8*k+2,n-4*k)/(3*n-8*k+2).
a(n) = sum(k=0, n\4, (-1)^k * (4*k+2) * binomial(3*n-8*k+2, n-4*k)/(3*n-8*k+2) );
for(n=0, 23, print1(a(n), ", "));



