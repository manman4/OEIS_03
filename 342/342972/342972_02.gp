M=8;

T(n, k) = prod(j=0, k-1, binomial(2*n-1, n+j)/binomial(2*n-1, j));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")))