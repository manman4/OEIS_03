M=26;

\\ a(n) = Sum_{k=0..n} binomial(n+4*k,n-1-k).
a(n) = sum(k=0, n, binomial(n+4*k, n-1-k));
for(n=1, M,print1(a(n),", "))
