M=20;

\\ a(n) = Sum_{k=0..n} 8^k * binomial(k-2/3,k) * binomial(2*n+1/3,n-k).
a(n) = sum(k=0, n, 8^k * binomial(k-2/3,k) * binomial(2*n+1/3,n-k));
for(n=0, M, print1(a(n)", "));