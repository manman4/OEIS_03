T(n, k) = if(n<k+1, sum(j=0, n, (-1)^(n-j)*binomial(2*j, j)*binomial(k*j, n-j)), ( 2 * sum(j=0, k, (-1)^j * binomial(k,j)*(2*n-1-j)*T(n-1-j,k)) )/n);
for(n=0, 10, for(k=0, n, print1(T(k, n-k),", ")))
