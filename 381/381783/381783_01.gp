M=21;

\\ a(n) = (1/(3*n+1)) * Sum_{k=0..n} binomial(3*n+1,k) * binomial(3*n-2*k+1,n-k).
a(n) = sum(k=0, n, binomial(3*n+1,k) * binomial(3*n-2*k+1,n-k)/(3*n+1));
for(n=0, M, print1(a(n), ", "))


 