a(n) = if(n==0, 1, sum(k=1, n, (5-4*k/n)*binomial(n, k)*a(n-k)));    
for(n=0, 15, print1(a(n),", "))    
 