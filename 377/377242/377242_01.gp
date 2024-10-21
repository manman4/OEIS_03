M=20;

\\ a(n) = (-9)^n * Sum_{k=0..n} binomial(-1/3,k) * binomial(n-4*k/3-1,n-k).
a(n) = (-9)^n * sum(k=0, n, binomial(-1/3, k)*binomial(n-4*k/3-1, n-k));
for(n=0, M, print1(a(n), ", ")) 