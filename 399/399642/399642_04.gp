\\ a(n) = ((n+1)/8) * A078995(n+1)
a078995(n) = sum(k=0, n, 3^(n-k)*binomial(4*n+1, k));
for(n=0, 30, print1(a078995(n), ", "));
a(n) = ((n+1)/8) * a078995(n+1);
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = ((n+1)/8) * Sum_{k=0..n+1} 3^(n+1-k) * binomial(4*n+5,k).
b(n) = ((n+1)/8) * sum(k=0, n+1, 3^(n+1-k) * binomial(4*n+5,k));
for(n=0, 30, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..n} 3^k * binomial(k+2,2) * binomial(4*n+5,n-k).
c(n) = sum(k=0, n, 3^k * binomial(k+2,2) * binomial(4*n+5,n-k));
for(n=0, 50, print1(a(n)-c(n), ", "));

