M=20;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..floor(n/3)} (n-3*k)^k * binomial(n+1,n-3*k)/(6^k * k!).
a(n) = (n!/(n+1)) * sum(k=0, n\3, (n-3*k)^k * binomial(n+1,n-3*k)/(6^k * k!));
for(n=0, M, print1(a(n),", "))   
