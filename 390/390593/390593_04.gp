M=22;

\\ a(n) = Sum_{k=0..n} (-1)^k * 2^(n-k) * binomial(5*n+5,k) * binomial(5*n-k+4,n-k).
a(n) = sum(k=0, n, (-1)^k * 2^(n-k) * binomial(5*n+5,k) * binomial(5*n-k+4,n-k));
for(n=0, M, print1(a(n),", "));  

