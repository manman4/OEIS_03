M=18;

\\ a(n) = 4 * Sum_{k=0..n} binomial(4*n+k+4,k) * binomial(4*n+k+4,n-k)/(4*n+k+4)
a(n) = 4 * sum(k=0, n, binomial(4*n+k+4, k) * binomial(4*n+k+4, n-k)/(4*n+k+4));
for(n=0, M, print1(a(n),", "))

\\ (1/(n+1)) * Sum_{k=0..n} binomial(4*n+k+3,k) * binomial(4*n+k+4,n-k).
b(n) = sum(k=0, n, binomial(4*n+k+3, k) * binomial(4*n+k+4, n-k)) / (n+1);
for(n=0, 100, print1(a(n)-b(n),", "))