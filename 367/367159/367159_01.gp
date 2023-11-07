a(n) = sum(k=0, n, (n+2*k)!/(n+k+1)! * abs(stirling(n, k, 1)));     
for(n=0, 17, print1(a(n),", "))         
