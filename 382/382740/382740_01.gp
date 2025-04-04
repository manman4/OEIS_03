M=10;

\\ A(n,k) = (1/2) * A382734(n,k).
a382734(n, k) = sum(j=0, min(n, k), j!*(j+1)!*stirling(n, j, 2)*stirling(k, j, 2));
a(n, k) = (1/2) * a382734(n, k);
for(n=1, M, for(k=1, n, print1(a(k, n-k+1)", ")));






