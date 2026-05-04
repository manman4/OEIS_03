\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 2^k * binomial(k+2,2) * binomial(3*n+3,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 2^k * binomial(k+2,2) * binomial(3*n+3,n-k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = (1/4) * Sum_{k=0..n+1} 2^(n+1-k) * binomial(3*n+2,k).
b(n) = (1/4) * sum(k=0, n+1, 2^(n+1-k) * binomial(3*n+2,k));
for(n=0, 50, print1(a(n)-b(n), ", "));
