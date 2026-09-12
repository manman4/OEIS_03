\\ a(n) = ((n+1)/6) * A006256(n+1) 
a006256(n) = sum(k=0, n, 2^(n-k)*binomial(3*n+1, k));
for(n=0, 30, print1(a006256(n), ", "));
a(n) = ((n+1)/6) * a006256(n+1);
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = ((n+1)/6) * Sum_{k=0..n+1} 2^(n+1-k) * binomial(3*n+4,k).
b(n) = ((n+1)/6) * sum(k=0, n+1, 2^(n+1-k) * binomial(3*n+4,k));
for(n=0, 30, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..n} 2^k * binomial(k+2,2) * binomial(3*n+4,n-k).
c(n) = sum(k=0, n, 2^k * binomial(k+2,2) * binomial(3*n+4,n-k));
for(n=0, 50, print1(a(n)-c(n), ", "));

