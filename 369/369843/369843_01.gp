M=29;

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(n+3*k,n-1-2*k).
a(n) = sum(k=0, n\2, binomial(n+3*k, n-1-2*k));
for(n=1, M,print1(a(n),", "))
