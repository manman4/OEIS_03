M=17;

\\ a(n) = Sum_{k=0..n} 3^k * 2^(n-k) * binomial(3*n+1,k).
a(n) = sum(k=0, n, 3^k * 2^(n-k) * binomial(3*n+1,k));
for(n=0, M, print1(a(n)", "));