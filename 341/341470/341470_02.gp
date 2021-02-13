T(n, k) = sum(j=0, n, 2^j*binomial(n, j)*binomial(k*n, j));
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")))