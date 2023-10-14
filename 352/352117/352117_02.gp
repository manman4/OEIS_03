M=18;

a(n) = sum(k=0, n, 2^(n-k)*prod(j=0, k-1, 2*j+1)*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));