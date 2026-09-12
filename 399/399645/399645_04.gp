\\ a(n) = ((n+1)/14) * A079563(n+1) 
a079563(n) = sum(k=0, n, 6^(n-k)*binomial(7*n+1, k));
for(n=0, 30, print1(a079563(n), ", "));
a(n) = ((n+1)/14) * a079563(n+1);
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = ((n+1)/14) * Sum_{k=0..n+1} 6^(n+1-k) * binomial(7*n+8,k).
b(n) = ((n+1)/14) * sum(k=0, n+1, 6^(n+1-k) * binomial(7*n+8,k));
for(n=0, 30, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..n} 6^k * binomial(k+2,2) * binomial(7*n+8,n-k).
c(n) = sum(k=0, n, 6^k * binomial(k+2,2) * binomial(7*n+8,n-k));
for(n=0, 50, print1(a(n)-c(n), ", "));

