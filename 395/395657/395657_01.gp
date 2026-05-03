\\ a(n) = A395660(n)/(n+1) = (1/(n+1)) * Sum_{k=0..n} 4^k * binomial(k+4,4) * binomial(5*n+5,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, 4^k * binomial(k+4,4) * binomial(5*n+5,n-k));
for(n=0, 20, print1(a(n), ", "));
