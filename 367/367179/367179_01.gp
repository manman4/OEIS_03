a(n) = sum(k=0, n, (3*n-2*k)!/(3*n-3*k+1)! * stirling(n, k, 1));     
for(n=0, 18, print1(a(n),", "))         
