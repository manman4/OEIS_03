a(n) = sum(k=0, n, (n+3*k)!/(n+2*k+1)! * stirling(n, k, 1));     
for(n=0, 17, print1(a(n),", "))         
