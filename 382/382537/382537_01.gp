M=24;

\\ a(n) = Sum_{k=0..n} 4^(n-k) * binomial(5*k/2,n-k).
a(n) = sum(k=0, n, 4^(n-k)*binomial(5*k/2,n-k));
for(n=0, M, print1(a(n),", "));


