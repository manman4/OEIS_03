M=20;

\\ a(n) = Sum_{k=0..n} 8^k * binomial(2*n,n-k).
a(n) = sum(k=0, n, 8^k * binomial(2*n, n-k));
for(n=0, M, print1(a(n)", "));