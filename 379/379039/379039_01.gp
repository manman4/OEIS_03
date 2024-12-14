M=20;

\\ a(n) = Sum_{k=0..n} binomial(4*k+2,k) * binomial(4*k+2,n-k)/(2*k+1).
a(n) = sum(k=0, n, binomial(4*k+2,k) * binomial(4*k+2,n-k)/(2*k+1));
for(n=0, M, print1(a(n),", "))