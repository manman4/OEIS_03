a(n) = sum(k=0, n, (-1)^(n-k) * prod(j=0, k-1, 4*j+2) * stirling(n, k, 2));
for(n=0, 17, print1(a(n),", "))    

b(n) = sum(k=0, n, (-1)^(n-k) * (2*k)! * stirling(n, k, 2)/k!);
for(n=0, 17, print1(a(n)-b(n),", "))    
