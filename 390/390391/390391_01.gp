M=17;

\\ a(n) = Sum_{k=0..n} binomial(n+7*k+7,k).
a(n) = sum(k=0, n, binomial(n+7*k+7, k));
for(n=0, M, print1(a(n),", "));
