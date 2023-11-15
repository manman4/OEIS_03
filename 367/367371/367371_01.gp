a(n) = sum(k=0, n, (-1)^(n-k) * prod(j=0, k-1, 3*j+2) * stirling(n, k, 2));
for(n=0, 18, print1(a(n),", "))    
