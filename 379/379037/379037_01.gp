M=21;

\\ a(n) = 2 * Sum_{k=0..n} binomial(3*k+2,k) * binomial(3*k+2,n-k)/(3*k+2).
a(n) = 2 * sum(k=0, n, binomial(3*k+2,k) * binomial(3*k+2,n-k)/(3*k+2));
for(n=0, M, print1(a(n),", "))