M=19;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 2^(n-k) * binomial(2*n+k+1,k) * binomial(2*n,n-k).
a(n) = sum(k=0, n, 2^(n-k) * binomial(2*n+k+1,k) * binomial(2*n,n-k)/(n+1));
for(n=0, M, print1(a(n)", "))