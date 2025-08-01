M=20;

\\ a(n) = Sum_{k=0..n} 3^k * binomial(n+k-1,k).
a(n) = sum(k=0, n, 3^k * binomial(n+k-1, k));
for(n=0, M, print1(a(n), ", "))

