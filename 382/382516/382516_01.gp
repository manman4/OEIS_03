M=20;

\\ a(n) = Sum_{k=0..n} 9^(n-k) * binomial(n+k/3-1,n-k).
a(n) = sum(k=0, n, 9^(n-k)*binomial(n+k/3-1,n-k));
for(n=0, M, print1(a(n),", "))
