M=38;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(5*k,k) * binomial(k,n-4*k) / (4*k+1).
a(n) = sum(k=0, n\4, binomial(5*k, k) * binomial(k, n-4*k) / (4*k+1));
for(n=0, M, print1(a(n), ", "));