T(n, k) = if(n==0, 1, sum(j=0, n-1, binomial(n,j) * binomial((k+1)*n-j,n-1-j) )/n);            
for(n=0, 10, for(k=0, n, print1(T(k,n-k),", ")))    
