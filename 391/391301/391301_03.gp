\\ a(n) = Sum_{k=0..n} (-1)^(floor((k+1)/2)) * (2*k+3) * binomial(3*n-k+3,n-k)/(3*n-k+3).
a(n) = sum(k=0, n, (-1)^((k+1)\2) * (2*k+3) * binomial(3*n-k+3, n-k)/(3*n-k+3));
for(n=0, 23, print1(a(n), ", "));

\\ a(n) = (1/(2*n+3)) * Sum_{k=0..n} (-1)^(floor((k+1)/2)) * (2*k+3) * binomial(3*n-k+2,n-k).
b(n) = (1/(2*n+3)) * sum(k=0, n, (-1)^((k+1)\2) * (2*k+3) * binomial(3*n-k+2, n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));

