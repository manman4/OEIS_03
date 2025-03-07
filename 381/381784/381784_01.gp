M=20;

\\ a(n) = (1/(4*n+1)) * Sum_{k=0..n} binomial(4*n+1,k) * binomial(4*n-2*k+1,n-k).
a(n) = sum(k=0, n, binomial(4*n+1,k) * binomial(4*n-2*k+1,n-k))/(4*n+1);
for(n=0, M, print1(a(n), ", "))


 