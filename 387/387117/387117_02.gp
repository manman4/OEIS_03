M=20;

\\ a(n) = Sum_{k=0..n} binomial(3*n+k,n).
a(n) = sum(k=0, n, binomial(3*n+k, n));
for(n=0, M, print1(a(n), ", "));
