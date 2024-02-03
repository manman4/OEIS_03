M=34;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n+k,n-2-4*k).
a(n) = sum(k=0, n\4, binomial(n+k, n-2-4*k));
for(n=1, M, print1(a(n),", "))
