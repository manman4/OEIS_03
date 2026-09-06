\\ a(n) = ((n+1)/14) * Sum_{k=0..n+1} 6^(n+1-k) * binomial(7*n+8,k).
a(n) = ((n+1)/14) * sum(k=0, n+1, 6^(n+1-k) * binomial(7*n+8,k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} 6^k * binomial(k+2,2) * binomial(7*n+8,n-k).
b(n) = sum(k=0, n, 6^k * binomial(k+2,2) * binomial(7*n+8,n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));

