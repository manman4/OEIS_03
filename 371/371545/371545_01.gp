M=22;

\\ a(n) = 2 * Sum_{k=0..n} (-1)^(n-k) * binomial(n-1,n-k) * binomial(4*k+2,k)/(4*k+2).
a(n) = 2 * sum(k=0, n, (-1)^(n-k) * binomial(n-1, n-k) * binomial(4*k+2, k)/(4*k+2));
for(n=0, M, print1(a(n),", "))  

