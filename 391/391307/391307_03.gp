\\ a(n) = Sum_{k=0..n} (-1)^(floor((k+1)/2)) * (2*k+4) * binomial(3*n-k+4,n-k)/(3*n-k+4).
a(n) = sum(k=0, n, (-1)^((k+1)\2) * (2*k+4) * binomial(3*n-k+4, n-k)/(3*n-k+4));
for(n=0, 23, print1(a(n), ", "));

\\ a(n) = (1/(n+2)) * Sum_{k=0..n} (-1)^(floor((k+1)/2)) * (k+2) * binomial(3*n-k+3,n-k).
b(n) = (1/(n+2)) * sum(k=0, n, (-1)^((k+1)\2) * (k+2) * binomial(3*n-k+3, n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));

