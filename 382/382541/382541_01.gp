M=22;

\\ a(n) = Sum_{k=0..n} 4^(n-k) * (k+1) * binomial(n+k/2-1,n-k).
a(n) = sum(k=0, n, 4^(n-k)*(k+1)*binomial(n+k/2-1,n-k));
for(n=0, M, print1(a(n),", "));


