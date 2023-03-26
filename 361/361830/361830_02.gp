T(n, k) = if(n<k+1, sum(j=0, n, binomial(2*j, j)*binomial(k*j, n-j)), ( 2 * sum(j=0, k, binomial(k,j)*(2*n-1-j)*T(n-1-j,k)) )/n);
for(n=0, 9, for(k=0, n, print1(T(k, n-k),", ")))
