M=22;

\\ a(n) = Sum_{k=0..n} 4^k * (-3)^(n-k) * binomial(2*n+2,k) * binomial(2*n-k,n-k).
a(n) = sum(k=0, n, 4^k * (-3)^(n-k) * binomial(2*n+2,k) * binomial(2*n-k,n-k));
for(n=0, M, print1(a(n)", "));