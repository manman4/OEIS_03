M=20;

\\ a(n) = Sum_{k=0..n} binomial(n+4*k+4,k).
a(n) = sum(k=0, n, binomial(n+4*k+4, k));
for(n=0, M, print1(a(n),", "));
