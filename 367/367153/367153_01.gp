a(n) = (2*n)! * sum(k=0, n, (-1)^(n-k) * stirling(n, k, 2)/(2*n-k+1)!);     
for(n=0, 19, print1(a(n),", "))         
