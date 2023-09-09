a(n) = sum(k=0, n, prod(j=0, k-1, 6*j+5)*stirling(n, k, 2));            
for(n=0, 16, print1(a(n),", "))   
 