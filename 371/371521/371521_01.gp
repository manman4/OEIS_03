M=19;

\\ a(n) = 6 * Sum_{k=0..n} binomial(n-1,n-k) * binomial(6*k+5,k)/(5*k+6).
a(n) = 6 * sum(k=0, n, binomial(n-1, n-k) * binomial(6*k+5, k)/(5*k+6));
for(n=0, M, print1(a(n),", "))  

\\ Sum_{k=0..n} binomial(n-1,n-k) * binomial(6*k+6,k)/(k+1).
b(n) = sum(k=0, n, binomial(n-1, n-k) * binomial(6*k+6, k)/(k+1));
for(n=0, M, print1(a(n)-b(n),", "))
