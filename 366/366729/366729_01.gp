a(n) = sum(k=0, n, (2*n-k)!/(2*n-2*k+1)! * stirling(n, k, 1));     
for(n=0, 19, print1(a(n),", "))         
