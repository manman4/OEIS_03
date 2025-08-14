M=21;

\\ a(n) = Sum_{k=0..n} binomial(4*n-3,k).
a(n) = sum(k=0, n, binomial(4*n-3,k));
for(n=0, M, print1(a(n)", "));