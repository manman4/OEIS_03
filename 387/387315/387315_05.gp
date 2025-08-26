M=25;

\\ a(n) = Sum_{k=0..n} 12^k * 13^(n-k) * binomial(-5/2,k) * binomial(n+4,n-k).
a(n) = sum(k=0, n, 12^k * 13^(n-k) * binomial(-5/2,k) * binomial(n+4,n-k));
for(n=0, M, print1(a(n), ", "));