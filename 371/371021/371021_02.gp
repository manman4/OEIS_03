M=22;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..floor(n/3)} k^(n-3*k) * binomial(n+1,k)/(6^k * (n-3*k)!).
a(n) = (n!/(n+1)) * sum(k=0, n\3, k^(n-3*k) * binomial(n+1,k)/(6^k * (n-3*k)!));
for(n=0, M, print1(a(n),", "))   
