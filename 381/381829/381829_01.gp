M=21;

\\ a(n) = (1/(3*n+1)) * Sum_{k=0..n} binomial(3*n+1,k) * binomial(4*n-3*k,n-k).
a(n) = sum(k=0, n, binomial(3*n+1,k) * binomial(4*n-3*k,n-k))/(3*n+1);
for(n=0, M, print1(a(n), ", "))

 