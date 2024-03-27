M=21;

\\ a(n) = Sum_{k=0..n} (-1)^(n-k) * binomial(n-1,n-k) * binomial(4*k+4,k)/(k+1).
a(n) = sum(k=0, n, (-1)^(n-k) * binomial(n-1, n-k) * binomial(4*k+4, k)/(k+1));
for(n=0, M, print1(a(n),", "))  

