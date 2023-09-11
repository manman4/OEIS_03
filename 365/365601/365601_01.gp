a(n) = sum(k=0, n, prod(j=0, k-1, 5*j+2)*stirling(n, k, 1));
for(n=0, 17, print1(a(n),", "))  

