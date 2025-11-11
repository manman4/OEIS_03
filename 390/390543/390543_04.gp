M=22;

\\ a(n) = Sum_{k=0..n} (-1)^k * 2^(n-k) * binomial(3*n+2,k) * binomial(3*n-k+1,n-k).
a(n) = sum(k=0, n, (-1)^k * 2^(n-k) * binomial(3*n+2,k) * binomial(3*n-k+1,n-k));
for(n=0, M, print1(a(n),", "));  

