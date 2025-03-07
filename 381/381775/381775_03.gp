M=17;

\\ a(n) = Sum_{k=0..n} binomial(6*n+2*k+1,k) * binomial(6*n+1,n-k)/(6*n+2*k+1).
a(n) = sum(k=0, n, binomial(6*n+2*k+1,k) * binomial(6*n+1,n-k)/(6*n+2*k+1));
for(n=0, M, print1(a(n), ", "))

 