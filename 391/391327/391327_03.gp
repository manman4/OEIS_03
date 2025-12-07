\\ a(n) = Sum_{k=0..n} (-1)^(floor((k+1)/2)) * (3*k+2) * binomial(4*n-k+2,n-k)/(4*n-k+2).
a(n) = sum(k=0, n, (-1)^((k+1)\2) * (3*k+2) * binomial(4*n-k+2, n-k)/(4*n-k+2));
for(n=0, 23, print1(a(n), ", "));

\\ a(n) = (1/(3*n+2)) * Sum_{k=0..n} (-1)^(floor((k+1)/2)) * (3*k+2) * binomial(4*n-k+1,n-k).
b(n) = (1/(3*n+2)) * sum(k=0, n, (-1)^((k+1)\2) * (3*k+2) * binomial(4*n-k+1, n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));

