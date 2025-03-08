M=22;

\\ a(n) = (1/(2*n+1)) * Sum_{k=0..n} binomial(2*n+1,k) * binomial(3*n-3*k,n-k).
a(n) = sum(k=0, n, binomial(2*n+1,k) * binomial(3*n-3*k,n-k))/(2*n+1)
for(n=0, M, print1(a(n), ", "))

 