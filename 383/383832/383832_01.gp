M=22;

\\ a(n) = Sum_{k=0..n} (k+1) * 3^k * binomial(2*n+2,n-k).
a(n) = sum(k=0, n, (k+1) * 3^k * binomial(2*n+2,n-k));
for(n=0, M, print1(a(n)", "));