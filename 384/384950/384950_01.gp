M=18;

\\ a(n) = Sum_{k=0..n} 3^k * binomial(2*n+k-1,k).
a(n) = sum(k=0, n, 3^k * binomial(2*n+k-1, k));
for(n=0, M, print1(a(n), ", "))

