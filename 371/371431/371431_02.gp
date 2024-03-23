M=24;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} (-1)^k * binomial(n+k,k) * binomial(3*n-k+1,n-3*k).
a(n) = sum(k=0, n\3, (-1)^k * binomial(n+k,k) * binomial(3*n-k+1,n-3*k)/(n+1));
for(n=0, M, print1(a(n)", "))