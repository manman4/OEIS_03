T(n, k) = sum(j=0, n, 2^(n-j)*binomial(k*n+1, j)*binomial((k+1)*n-j, n-j))/(k*n+1); 
for(n=0, 10, for(k=0, n, print1(T(k, n-k),", ")))

