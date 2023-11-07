a(n) = (3*n)! * sum(k=0, n, (-1)^(n-k) * stirling(n, k, 2)/(3*n-k+1)!);     
for(n=0, 17, print1(a(n),", "))         
