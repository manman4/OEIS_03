M=21;

\\ a(n) = Sum_{k=0..n} (-1)^k * binomial(4*n-k+1,n-k).
a(n) = sum(k=0, n, (-1)^k * binomial(4*n-k+1,n-k));
for(n=0, M, print1(a(n),", "));  

