M=22;

\\ a(n) = Sum_{k=0..n} (-9)^k * binomial(-1/3,k) * binomial(2*n-k,n-k).
a(n) = sum(k=0, n, (-9)^k * binomial(-1/3,k) * binomial(2*n-k,n-k));
for(n=0, M, print1(a(n)", "));