M=10;

\\ A(n,k) = (1/4) * A382736(n,k).
a382736(n, k) = sum(j=0, min(n, k), j!^2*binomial(j+3, 3)*stirling(n, j, 2)*stirling(k, j, 2));
a(n, k) = (1/4) * a382736(n, k);
for(n=1, M, for(k=1, n, print1(a(k, n-k+1)", ")));






