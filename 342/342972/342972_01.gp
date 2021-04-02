M=8;

T(n, k) = prod(j=0, n-1, binomial(n+j, k)/binomial(k+j, k));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")))