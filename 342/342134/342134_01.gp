T(n, k) = sum(j=0, n\2, (2*k)^(n-j)*2^(-j)*binomial(n-j, j));
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")))