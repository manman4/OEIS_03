\\ a(0) = 1; a(n) = (9/n) * Sum_{k=0..n-1} 2^k * binomial(k+3,3) * binomial(3*n+2,n-1-k). 
a(n) = if(n==0, 1, (9/n) * sum(k=0, n-1, 2^k * binomial(k+3,3) * binomial(3*n+2,n-1-k)));
for(n=0, 20, print1(a(n), ", "));