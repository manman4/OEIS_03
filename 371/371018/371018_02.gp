M=19;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..floor(n/2)} k^(n-2*k) * binomial(n+1,k)/(n-2*k)!.
a(n) = (n!/(n+1)) * sum(k=0, n\2, k^(n-2*k) * binomial(n+1,k)/((n-2*k)!));
for(n=0, M, print1(a(n),", "))   
