\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * (4*k+3) * binomial(3*n-2*k+3,n-2*k)/(3*n-2*k+3).
a(n) = sum(k=0, n\2, (-1)^k * (4*k+3) * binomial(3*n-2*k+3, n-2*k)/(3*n-2*k+3) );
for(n=0, 23, print1(a(n), ", "));

\\ a(n) = (1/(2*n+3)) * Sum_{k=0..floor(n/2)} (-1)^k * (4*k+3) * binomial(3*n-2*k+2,n-2*k).
b(n) = (1/(2*n+3)) * sum(k=0, n\2, (-1)^k * (4*k+3) * binomial(3*n-2*k+2, n-2*k) );
for(n=0, 50, print1(a(n)-b(n), ", "));

