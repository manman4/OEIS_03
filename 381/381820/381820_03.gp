M=18;

\\ a(n) = Sum_{k=0..n} binomial(4*n+2*k+1,k) * binomial(5*n-k,n-k)/(4*n+2*k+1).
a(n) = sum(k=0, n, binomial(4*n+2*k+1,k) * binomial(5*n-k,n-k)/(4*n+2*k+1));
for(n=0, M, print1(a(n), ", "))

 