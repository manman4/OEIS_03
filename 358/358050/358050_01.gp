M=10;

T(n, k) = sum(j=0, n, binomial(k*j, j)*binomial(k*(n-j), n-j));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));