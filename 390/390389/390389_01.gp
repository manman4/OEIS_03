M=19;

\\ a(n) = Sum_{k=0..n} binomial(n+5*k+5,k).
a(n) = sum(k=0, n, binomial(n+5*k+5, k));
for(n=0, M, print1(a(n),", "));
