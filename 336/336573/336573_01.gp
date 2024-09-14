T(n, k) = (-1)^n*sum(j=0, n, (-2)^j*binomial(n, j)*binomial(k*n+j+1, n)/(k*n+j+1)); 
for(n=0, 10, for(k=0, n, print1(T(k,n-k),", ")))    
