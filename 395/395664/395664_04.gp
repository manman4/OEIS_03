\\ a(n) = ((n+1)/6) * Sum_{k=0..n+1} 3^(n+1-k) * binomial(4*n+3,k).
a(n) = ((n+1)/6) * sum(k=0, n+1, 3^(n+1-k) * binomial(4*n+3,k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} 3^k * binomial(k+2,2) * binomial(4*n+4,n-k).
b(n) = sum(k=0, n, 3^k * binomial(k+2,2) * binomial(4*n+4,n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));
