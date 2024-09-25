M=37;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(5*k,k) * binomial(n-3*k-1,n-4*k) / (4*k+1).
a(n) = sum(k=0, n\4, binomial(5*k, k) * binomial(n-3*k-1, n-4*k) / (4*k+1));
for(n=0, M, print1(a(n), ", "));