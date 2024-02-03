M=25;

\\ a(n) = Sum_{k=0..n} binomial(n+2+4*k,n-1-k).
a(n) = sum(k=0, n, binomial(n+2+4*k, n-1-k));
for(n=1, M,print1(a(n),", "))
