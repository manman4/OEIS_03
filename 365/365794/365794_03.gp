a(n) = if(n==0, 1, sum(k=1, n, 2^k * (2 - 1/2 * k/n) * binomial(n,k) * a(n-k)));  
for(n=0, 15, print1(a(n),", ")) 

