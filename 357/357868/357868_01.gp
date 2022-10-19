M=11;

T(n, k) = sum(j=0, n, (k*j)!*stirling(n, k*j, 2));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));