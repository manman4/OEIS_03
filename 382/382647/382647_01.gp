M=27;

\\ a(n) = Sum_{k=0..n} 4^(n-k) * (k+1) * binomial(k/2,n-k).
a(n) = sum(k=0, n, (k+1)*4^(n-k)*binomial(k/2,n-k));
for(n=0, M, print1(a(n),", "));




