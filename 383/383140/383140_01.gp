M=10;

T(n, k) = sum(j=k, n, 3^(n-j)*stirling(n, j, 2)*stirling(j, k, 1));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")));
