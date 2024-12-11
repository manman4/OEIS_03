a(n) = if(n==0, 1, sum(k=1, n, (-1)^(n-k) * binomial(n,k) * binomial(3*n,k-1) )/n); 
for(n=0, 27, print1(a(n),", "))          
