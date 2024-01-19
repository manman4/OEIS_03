\\ a(n) = (1/(n+1)) * Sum_{k=0..n} (-1)^k * binomial(3*n+k+2,k) * binomial(7*n-k+5,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, (-1)^k * binomial(3*n+k+2,k) * binomial(7*n-k+5,n-k));
for(n=0, 20, print1(a(n),", "))  

