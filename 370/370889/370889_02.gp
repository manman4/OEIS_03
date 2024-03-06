M=19;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..floor(n/2)} (n-2*k)^k * binomial(n+1,n-2*k)/(2^k * k!).
a(n) = (n!/(n+1)) * sum(k=0, n\2, (n-2*k)^k * binomial(n+1,n-2*k)/(2^k * k!));
for(n=0, M, print1(a(n),", "))   
