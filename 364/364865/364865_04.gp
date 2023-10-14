a(n) = if(n==0, 1, sum(k=1, n, (-1)^(n-k) * binomial(n,k) * binomial(4*n,k-1) )/n); 
for(n=0, 24, print1(a(n),", "))          

