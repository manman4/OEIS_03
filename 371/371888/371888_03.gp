a(n) = if(n==0, 1, 1/2 * sum(k=0, n, 4^k * binomial(k/2+1/2,k) * binomial(n-1,n-k)/(k+1) ));
for(n=0, 20, print1(a(n), ", "))