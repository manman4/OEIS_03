M=9;

T(n, k) = sum(j=0, 2*n, (-1)^(n+j)*binomial(2*n, j)^k);
for(n=0, M, for(k=0, n, print1(T(k, n-k),", ")));