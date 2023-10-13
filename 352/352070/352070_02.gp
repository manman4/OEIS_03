M=19;

a(n) = sum(k=0, n, 3^(n-k)*prod(j=0, k-1, 3*j+1)*stirling(n, k, 1));
for(n=0, M, print1(a(n), ", "));