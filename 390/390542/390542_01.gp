M=22;

\\ a(n) = Sum_{k=0..n} (-1)^(n-k) * binomial(n+2*k+1,k).
a(n) = sum(k=0, n, (-1)^(n-k) * binomial(n+2*k+1,k));
for(n=0, M, print1(a(n),", "));  

