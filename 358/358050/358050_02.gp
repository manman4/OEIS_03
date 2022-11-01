M=10;

T(n, k) = sum(j=0, n, (k-1)^(n-j)*binomial(k*n+1, j));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));