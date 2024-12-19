M=17;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(3*n+7*k+1,n)/(3*n+7*k+1).
a(n) = sum(k=0, n, binomial(n, k) * binomial(3*n+7*k+1, n)/(3*n+7*k+1)); 
for(n=0, M, print1(a(n),", "))
