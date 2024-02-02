M=31;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n-1+3*k,n-4*k).
a(n) = sum(k=0, n\4, binomial(n-1+3*k, n-4*k));
for(n=0, M, print1(a(n), ", "));