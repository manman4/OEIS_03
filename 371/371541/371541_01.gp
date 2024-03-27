M=19;

\\ a(n) = Sum_{k=0..n} (-1)^(n-k) * binomial(n-1,n-k) * binomial(6*k+6,k)/(k+1).
a(n) = sum(k=0, n, (-1)^(n-k) * binomial(n-1, n-k) * binomial(6*k+6, k)/(k+1));
for(n=0, M, print1(a(n),", "))  

