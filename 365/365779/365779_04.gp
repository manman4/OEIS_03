a(n) = if(n==0, 1, 5*a(n-1) + sum(k=1, n-1, 3^k * binomial(n-1,k) * a(n-k)));   
for(n=0, 17, print1(a(n),", "))     
