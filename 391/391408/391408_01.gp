M=26;
\\ a(n) = Sum_{k=0..n} (-1)^k * (k+1) * (k+5) * binomial(2*n-k+5,n-k)/(2*n-k+5).
a(n) = sum(k=0, n, (-1)^k * (k+1) * (k+5) * binomial(2*n-k+5, n-k)/(2*n-k+5));
for(n=0, M, print1(a(n),", "));

\\ a(n) = (1/(n+5)) * Sum_{k=0..n} (-1)^k * (k+1) * (k+5) * binomial(2*n-k+4,n-k).
b(n) = (1/(n+5)) * sum(k=0, n, (-1)^k * (k+1) * (k+5) * binomial(2*n-k+4, n-k));
for(n=0, 100, print1(a(n)-b(n),", "));