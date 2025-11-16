M=21;
\\ a(n) = Sum_{k=0..n} (5*k+1) * binomial(4*n+k+1,n-k)/(4*n+k+1).
a(n) = sum(k=0, n, (5*k+1) * binomial(4*n+k+1, n-k)/(4*n+k+1));
for(n=0, M, print1(a(n),", "))  