T(n, k) = sum(j=0, n, binomial(k*n, n-j)*binomial(k*n+j, j));
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")))