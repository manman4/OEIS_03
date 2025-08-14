M=25;

\\ a(n) = Sum_{k=0..n} 2^k * (-1)^(n-k) * binomial(4*n-2,k) * binomial(4*n-k-3,n-k).
a(n) = sum(k=0, n, 2^k * (-1)^(n-k) * binomial(4*n-2,k) * binomial(4*n-k-3,n-k));
for(n=0, M, print1(a(n)", "));