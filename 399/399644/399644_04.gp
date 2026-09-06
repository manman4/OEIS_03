\\ a(n) = ((n+1)/12) * Sum_{k=0..n+1} 5^(n+1-k) * binomial(6*n+7,k).
a(n) = ((n+1)/12) * sum(k=0, n+1, 5^(n+1-k) * binomial(6*n+7,k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} 5^k * binomial(k+2,2) * binomial(6*n+7,n-k).
b(n) = sum(k=0, n, 5^k * binomial(k+2,2) * binomial(6*n+7,n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));
