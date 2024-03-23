M=25;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} (-1)^k * binomial(n+k,k) * binomial(4*n+2*k+2,n-k).
a(n) = sum(k=0, n, (-1)^k * binomial(n+k,k) * binomial(4*n+2*k+2,n-k)/(n+1));
for(n=0, M, print1(a(n)", "))