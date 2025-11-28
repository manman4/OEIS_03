\\ a(n) = Sum_{k=0..n} (-1)^k * (3*k+3) * binomial(4*n-k+3,n-k)/(4*n-k+3).
a(n) = sum(k=0, n, (-1)^k*(3*k+3)*binomial(4*n-k+3, n-k)/(4*n-k+3));
for(n=0, 21, print1(a(n), ", "));

\\ a(n) = (1/(3*n+3)) * Sum_{k=0..n} (-1)^k * (3*k+3) * binomial(4*n-k+2,n-k).
b(n) = sum(k=0, n, (-1)^k*(3*k+3)*binomial(4*n-k+2, n-k))/(3*n+3);
for(n=0, 100, print1(a(n)-b(n), ", "));

