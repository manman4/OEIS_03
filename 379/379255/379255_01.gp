M=17;

\\ a(n) = Sum_{k=0..n} binomial(3*n+7*k+2,k) * binomial(3*n+6*k+3,n-k)/(n+2*k+1).
a(n) = sum(k=0, n, binomial(3*n+7*k+2, k) * binomial(3*n+6*k+3, n-k)/(n+2*k+1));
for(n=0, M, print1(a(n),", "))

