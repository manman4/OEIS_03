a(n) = sum(k=0, n, (-1)^(n-k) * (3*k)!/(2*k+1)! * stirling(n, k, 2));     
for(n=0, 17, print1(a(n),", "))         
