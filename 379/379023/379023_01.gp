M=19;

\\ a(n) = 3 * Sum_{k=0..n} binomial(3*n+k+3,k) * binomial(3*n+k+3,n-k)/(3*n+k+3)
a(n) = 3 * sum(k=0, n, binomial(3*n+k+3, k) * binomial(3*n+k+3, n-k)/(3*n+k+3));
for(n=0, M, print1(a(n),", "))

\\ (1/(n+1)) * Sum_{k=0..n} binomial(3*n+k+2,k) * binomial(3*n+k+3,n-k).
b(n) = sum(k=0, n, binomial(3*n+k+2, k) * binomial(3*n+k+3, n-k)) / (n+1);
for(n=0, 100, print1(a(n)-b(n),", "))