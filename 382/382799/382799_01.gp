M=10;

a(n, k) = sum(j=0, min(n, k), j!*(j+1)!*abs(stirling(n, j, 1)*stirling(k, j, 1)));
for(n=0, M, for(k=0, n, print1(a(k, n-k)", ")));






