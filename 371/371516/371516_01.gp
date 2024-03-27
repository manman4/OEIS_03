M=22;

\\ a(n) = 3 * Sum_{k=0..n} binomial(n-1,n-k) * binomial(3*k+2,k)/(2*k+3).
a(n) = 3 * sum(k=0, n, binomial(n-1, n-k) * binomial(3*k+2, k)/(2*k+3));
for(n=0, M, print1(a(n),", "))  

\\ Sum_{k=0..n} binomial(n-1,n-k) * binomial(3*k+3,k)/(k+1).
b(n) = sum(k=0, n, binomial(n-1, n-k) * binomial(3*k+3, k)/(k+1));
for(n=0, M, print1(a(n)-b(n),", "))
