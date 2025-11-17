M=22;
\\ a(n) = Sum_{k=0..n} (-1)^k * (4*k+1) * binomial(5*n-k+1,n-k)/(5*n-k+1).
a(n) = sum(k=0, n, (-1)^k * (4*k+1) * binomial(5*n-k+1, n-k)/(5*n-k+1));
for(n=0, M, print1(a(n),", "))  