a(n) = sum(k=0, n, (n+3*k+1)^(k-1) * stirling(n, k, 2));     
for(n=0, 16, print1(a(n),", "))         
