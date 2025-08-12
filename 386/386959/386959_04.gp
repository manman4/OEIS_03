M=22;

\\ a(n) = Sum_{k=0..n} 9^k * binomial(k-2/3,k) * binomial(2*n-k-1,n-k).
a(n) = sum(k=0, n, 9^k * binomial(k-2/3,k) * binomial(2*n-k-1,n-k));
for(n=0, M, print1(a(n)", "));