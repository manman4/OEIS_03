M=29;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} (-1)^k * binomial(2*n-2*k+1,k) * binomial(2*n-2*k,n-2*k).
a(n) = sum(k=0, n, (-1)^k * binomial(2*n-2*k+1,k) * binomial(2*n-2*k,n-2*k)) / (n+1);
for(n=0, M, print1(a(n), ", "))