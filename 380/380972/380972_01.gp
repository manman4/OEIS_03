M=18;

\\ a(n) = Sum_{k=0..n} (2*k)^(n-k) * (n+1)^(k-1) * binomial(n,k).
a(n) = sum(k=0, n, (2*k)^(n-k) * (n+1)^(k-1) * binomial(n,k));
for(n=0, M, print1(a(n),", "));

