M=11;

T(n, k) = sum(j=1, n, sigma(j, k)*(n\j));
for(n=0, M, for(k=1, n, print1(T(k, n-k), ", ")));