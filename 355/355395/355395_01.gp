M=10;

T(n, k) = sum(j=0, n, k^(j*(n-j))*binomial(n, j));
for(n=0, M, for(k=0, n, print1(T(k, n-k),", ")))  