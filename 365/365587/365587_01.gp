a(n) = sum(k=0, n, prod(j=0, k-1, 5*j+4)*abs(stirling(n, k, 1)));
for(n=0, 16, print1(a(n),", "))  

