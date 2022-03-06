M=17;

a(n) = sum(k=0, n, (-4)^(n-k)*prod(j=0, k-1, -4*j+1)*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));