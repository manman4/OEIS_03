a(n) = sum(k=0, n, prod(j=0, k-1, 4*j+3)*abs(stirling(n, k, 1)));
for(n=0, 17, print1(a(n),", "))  

