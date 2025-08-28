\\ a(n) = Sum_{k=0..n} 2^k * 3^(n-k) * binomial(n+1,k) * binomial(n+1,n-k).
a(n) = sum(k=0, n, 2^k * 3^(n-k) * binomial(n+1,k) * binomial(n+1,n-k));
for(n=0, 20, print1(a(n),", "));

\\ a(n) = Sum_{k=0..n} 3^k * 2^(n-k) * binomial(n+1,k) * binomial(n+1,n-k).
b(n) = sum(k=0, n, 3^k * 2^(n-k) * binomial(n+1,k) * binomial(n+1,n-k));
for(n=0, 50, print1(a(n)-b(n),", "));