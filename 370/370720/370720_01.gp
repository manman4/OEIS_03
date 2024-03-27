M=23;

\\ a(n) = 4 * Sum_{k=0..n} (-1)^(n-k) * binomial(n-1,n-k) * binomial(3*k+4,k)/(3*k+4).
a(n) = 4 * sum(k=0, n, (-1)^(n-k) * binomial(n-1, n-k) * binomial(3*k+4, k)/(3*k+4));
for(n=0, M, print1(a(n),", "))  

