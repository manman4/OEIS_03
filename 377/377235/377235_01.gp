M=20;

\\ a(n) = Sum_{k=0..n} (-9)^k * binomial(-5/3,k) * binomial(n-1,n-k).
a(n) = sum(k=0, n, (-9)^k*binomial(-5/3, k)*binomial(n-1, n-k));
for(n=0, M, print1(a(n), ", ")) 