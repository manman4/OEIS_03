\\ a(n) = Sum_{k=0..n} (-1)^(floor((k+1)/2)) * (2*k+2) * binomial(3*n-k+2,n-k)/(3*n-k+2).
a(n) = sum(k=0, n, (-1)^((k+1)\2) * (2*k+2) * binomial(3*n-k+2, n-k)/(3*n-k+2));
for(n=0, 24, print1(a(n), ", "));

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} (-1)^(floor((k+1)/2)) * (k+1) * binomial(3*n-k+1,n-k).
b(n) = (1/(n+1)) * sum(k=0, n, (-1)^((k+1)\2) * (k+1) * binomial(3*n-k+1, n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));

