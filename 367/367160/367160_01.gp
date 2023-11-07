a(n) = sum(k=0, n, (n+3*k)!/(n+2*k+1)! * abs(stirling(n, k, 1)));     
for(n=0, 16, print1(a(n),", "))         
