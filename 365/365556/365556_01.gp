a(n) = sum(k=0, n, prod(j=0, k-1, 6*j+4)*stirling(n, k, 2));
for(n=0, 16, print1(a(n),", "))   

b(n) = sum(k=0, n, 2^k * prod(j=0, k-1, 3*j+2)*stirling(n, k, 2));
for(n=0, 16, print1(a(n)-b(n),", "))   