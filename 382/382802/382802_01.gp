M=9;

a(n, k) = sum(j=0, min(n, k), j!^2*binomial(j+2, 2)*abs(stirling(n, j, 1)*stirling(k, j, 1)))/3;
for(n=1, M, for(k=1, n, print1(a(k, n-k+1)", ")));



