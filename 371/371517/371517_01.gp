M=20;

\\ a(n) = 4 * Sum_{k=0..n} binomial(n-1,n-k) * binomial(4*k+3,k)/(3*k+4).
a(n) = 4 * sum(k=0, n, binomial(n-1, n-k) * binomial(4*k+3, k)/(3*k+4));
for(n=0, M, print1(a(n),", "))  

\\ Sum_{k=0..n} binomial(n-1,n-k) * binomial(4*k+4,k)/(k+1).
b(n) = sum(k=0, n, binomial(n-1, n-k) * binomial(4*k+4, k)/(k+1));
for(n=0, M, print1(a(n)-b(n),", "))
