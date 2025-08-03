M=20;

\\ a(n) = Sum_{k=0..n} binomial(5*n+1,k).
a(n) = sum(k=0, n, binomial(5*n+1,k));
for(n=0, M, print1(a(n)", "));