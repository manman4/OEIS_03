a(n) = (2*n)! * sum(k=0, n, abs(stirling(n, k, 1))/(2*n-k+1)!);     
for(n=0, 17, print1(a(n),", "))         
