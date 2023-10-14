M=10;

T(n, k) = sum(j=0, n, binomial(n, j)^k); 
for(n=0, M, for(k=0, n, print1(T(k, n-k),", ")));