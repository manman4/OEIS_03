M=18;

\\ a(n) = Sum_{k=0..n} binomial(n+6*k,k).
a(n) = sum(k=0, n, binomial(n+6*k, k));
for(n=0, M, print1(a(n),", "));
