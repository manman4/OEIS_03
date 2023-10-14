T(n, k) = sum(j=0, n, k^j*binomial(n, j)*binomial(2*j, j));
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")))