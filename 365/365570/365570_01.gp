a(n) = sum(k=0, n, prod(j=0, k-1, 5*j+4)*stirling(n, k, 2));        
for(n=0, 16, print1(a(n),", "))   
 