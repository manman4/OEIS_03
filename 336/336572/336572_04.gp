a(n) = if(n==0, 1, sum(k=0, n-1, (-1)^k * 3^(n-k) * binomial(n,k) * binomial(5*n-k,n-1-k) )/n); 
for(n=0, 19, print1(a(n),", "))          

