M=27;

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(n-1+5*k,n-2*k).
a(n) = sum(k=0, n\2, binomial(n-1+5*k, n-2*k));
for(n=0, M, print1(a(n), ", "));