\\ a(0) = 1; a(n) = (16/n) * Sum_{k=0..n-1} 3^k * binomial(k+4,4) * binomial(4*n+3,n-1-k).
a(n) = if(n==0, 1, (16/n) * sum(k=0, n-1, 3^k * binomial(k+4,4) * binomial(4*n+3,n-1-k)));
for(n=0, 20, print1(a(n), ", "));