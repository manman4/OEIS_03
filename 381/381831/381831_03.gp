M=21;

\\ a(n) = Sum_{k=0..n} binomial(3*n+k+1,k) * binomial(4*n-2*k,n-k)/(3*n+k+1).
a(n) = sum(k=0, n, binomial(3*n+k+1,k) * binomial(4*n-2*k,n-k)/(3*n+k+1));
for(n=0, M, print1(a(n), ", "))

 