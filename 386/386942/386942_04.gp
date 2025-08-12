M=22;

\\ a(n) = Sum_{k=0..n} 3^(n-k) * binomial(2*n+3/2,k) * binomial(n-k+1/2,n-k).
a(n) = sum(k=0, n, 3^(n-k) * binomial(2*n+3/2,k) * binomial(n-k+1/2,n-k));
for(n=0, M, print1(a(n)", "));

\\ Sum_{k=0..n} (2*k+1) * (3/4)^k * binomial(2*k,k) * binomial(2*n+3/2,n-k).
b(n) = sum(k=0, n, (2*k+1) * (3/4)^k * binomial(2*k,k) * binomial(2*n+3/2,n-k));
for(n=0, 50, print1(a(n)-b(n)", "));