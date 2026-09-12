\\ a(n) = ((n+1)/12) * A079679(n+1)
a079679(n) = sum(k=0, n, 5^(n-k)*binomial(6*n+1, k));
for(n=0, 30, print1(a079679(n), ", "));
a(n) = ((n+1)/12) * a079679(n+1);
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = ((n+1)/12) * Sum_{k=0..n+1} 5^(n+1-k) * binomial(6*n+7,k).
b(n) = ((n+1)/12) * sum(k=0, n+1, 5^(n+1-k) * binomial(6*n+7,k));
for(n=0, 30, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..n} 5^k * binomial(k+2,2) * binomial(6*n+7,n-k).
c(n) = sum(k=0, n, 5^k * binomial(k+2,2) * binomial(6*n+7,n-k));
for(n=0, 50, print1(a(n)-c(n), ", "));

