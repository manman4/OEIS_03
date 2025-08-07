M=25;

\\ a(n) = Sum_{k=0..n} 5^k * 3^(n-k) * binomial(3*n-k,n-k).
a(n) = sum(k=0, n, 5^k * 3^(n-k) * binomial(3*n-k,n-k));
for(n=0, M, print1(a(n)", "));

