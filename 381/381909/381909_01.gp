M=20;

\\ a(n) = Sum_{k=0..n} binomial(n+4*k+1,k) * binomial(2*n+2,n-k)/(n+4*k+1).
a(n) = sum(k=0, n, binomial(n+4*k+1, k) * binomial(2*n+2, n-k) / (n+4*k+1));
for(n=0, M, print1(a(n), ", "));

 