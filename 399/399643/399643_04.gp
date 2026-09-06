\\ a(n) = ((n+1)/10) * Sum_{k=0..n+1} 4^(n+1-k) * binomial(5*n+6,k).
a(n) = ((n+1)/10) * sum(k=0, n+1, 4^(n+1-k) * binomial(5*n+6,k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} 4^k * binomial(k+2,2) * binomial(5*n+6,n-k).
b(n) = sum(k=0, n, 4^k * binomial(k+2,2) * binomial(5*n+6,n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));
