M=18;

\\ a(n) = Sum_{k=0..n} binomial(n+6*k+6,k).
a(n) = sum(k=0, n, binomial(n+6*k+6, k));
for(n=0, M, print1(a(n),", "));
