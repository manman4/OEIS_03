\\ a(n) = ((n+1)/10) * Sum_{k=0..n+1} 5^(n+1-k) * binomial(6*n+5,k).
a(n) = ((n+1)/10) * sum(k=0, n+1, 5^(n+1-k) * binomial(6*n+5,k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} 5^k * binomial(k+2,2) * binomial(6*n+6,n-k).
b(n) = sum(k=0, n, 5^k * binomial(k+2,2) * binomial(6*n+6,n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));
