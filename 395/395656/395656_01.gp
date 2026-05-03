\\ a(n) = A395659(n)/(n+1) = (1/(n+1)) * Sum_{k=0..n} 3^k * binomial(k+3,3) * binomial(4*n+4,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 3^k * binomial(k+3,3) * binomial(4*n+4,n-k));
for(n=0, 20, print1(a(n), ", "));
