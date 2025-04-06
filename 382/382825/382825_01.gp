M=9;

a(n, k) = sum(j=0, min(n, k), j!^2*binomial(j+2, 2)*abs(stirling(n+1, j+1, 1)*stirling(k+1, j+1, 1)));
for(n=0, M, for(k=0, n, print1(a(k, n-k)", ")));






