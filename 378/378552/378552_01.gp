M=18;

\\ a(n) = Sum_{k=0..n} 9^k * binomial(n/3+k-1,k) * binomial(n-1,n-k).
a(n) = sum(k=0, n, 9^k * binomial(n/3+k-1, k) * binomial(n-1, n-k));
for(n=0, M, print1(a(n), ", "));