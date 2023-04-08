T(n, k) = sum(j=0, n, binomial(j+k-1, j)*binomial(n*j, n-j));

for(n=0, 10, for(k=0, n, print1(T(k, n-k),", ")))  