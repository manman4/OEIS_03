\\ a(n) = (1/12) * Sum_{k=0..n+1} 6^(n+1-k) * binomial(7*n+6,k).
a(n) = (1/12) * sum(k=0, n+1, 6^(n+1-k) * binomial(7*n+6,k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 6^k * binomial(k+2,2) * binomial(7*n+7,n-k).
b(n) = (1/(n+1)) * sum(k=0, n, 6^k * binomial(k+2,2) * binomial(7*n+7,n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));
