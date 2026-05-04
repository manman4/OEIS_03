\\ a(n) = (n+1) * Sum_{k=0..n} 3^(n-k) * binomial(4*n+4,k).
a(n) = (n+1) * sum(k=0, n, 3^(n-k) * binomial(4*n+4,k));
for(n=0, 25, print1(a(n), ", "));

\\ a(n) = (n+1) * Sum_{k=0..n} 4^(n-k) * binomial(3*n+k+3,k).
b(n) = (n+1) * sum(k=0, n, 4^(n-k) * binomial(3*n+k+3,k));
for(n=0, 25, print1(a(n)-b(n), ", "));