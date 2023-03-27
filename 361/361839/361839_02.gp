T(n, k) = if(n<k+1, sum(j=0, n, (-9)^j*binomial(-1/3, j)*binomial(k*j, n-j)), 3 * sum(j=0, k, binomial(k,j)*(3*n-2-2*j)*T(n-1-j,k))/n);
for(n=0, 9, for(k=0, n, print1(T(k,n-k),", ")))    