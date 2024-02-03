M=25;

\\ a(n) = sum(k=0, n, binomial(n+4*k, n-k));
a(n) = sum(k=0, n, binomial(n+4*k, n-k));
for(n=0, M,print1(a(n),", "))
