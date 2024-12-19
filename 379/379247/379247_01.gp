M=17;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(4*n+5*k+1,n)/(4*n+5*k+1).
a(n) = sum(k=0, n, binomial(n, k) * binomial(4*n+5*k+1, n)/(4*n+5*k+1));
for(n=0, M, print1(a(n),", "))
