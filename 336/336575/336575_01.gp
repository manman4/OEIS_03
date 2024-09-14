T(n, k) = if(n==0, 1, sum(j=1, n, 3^j*binomial(n, j)*binomial(k*n, j-1))/n); 
for(n=0, 10, for(k=0, n, print1(T(k, n-k),", ")))

