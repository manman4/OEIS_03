M=21;

\\ a(n) = Sum_{k=0..n} 9^(n-k) * binomial(n+k-1,k) * binomial(k/3,n-k).
a(n) = sum(k=0, n, 9^(n-k) * binomial(n+k-1, k) * binomial(k/3, n-k));
for(n=0, M, print1(a(n), ", "));