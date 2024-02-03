M=31;

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(n+1+2*k,n-2-3*k).
a(n) = sum(k=0, n\3, binomial(n+1+2*k, n-2-3*k));
for(n=1, M, print1(a(n),", "))
