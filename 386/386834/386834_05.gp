M=25;

\\ a(n) = Sum_{k=0..n} 2^k * (n-k+1) * binomial(3*n+k-1,k).
a(n) = sum(k=0, n, 2^k * (n-k+1) * binomial(3*n+k-1, k));
for(n=0, M, print1(a(n), ", "))

