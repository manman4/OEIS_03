M=11;

T(n, k) = n!*sum(j=0, n\(k+1), stirling(n-k*j, j, 1)/(n-k*j)!);
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")))