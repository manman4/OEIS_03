M=10;

T(n, k) = sum(j=0, n, k^(n-j)*binomial((k-1)*n+j, j));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));