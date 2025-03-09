M=23;

\\ a(n) = Sum_{k=0..n} binomial(3*k+1,k) * binomial(n+k+1,n-k)/(3*k+1).
a(n) = sum(k=0, n, binomial(3*k+1,k) * binomial(n+k+1,n-k)/(3*k+1));
for(n=0, M, print1(a(n), ", "))

 