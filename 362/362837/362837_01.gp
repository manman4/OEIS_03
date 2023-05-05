M=10;

T(n, k) = (-1)^n*n!*sum(j=0, n\2, k^(n-j)*stirling(n-j, j, 1)/(n-j)!);
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
