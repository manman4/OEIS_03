a(n) = sum(k=0, n, (-3)^(n-k) * prod(j=0, k-1, 6*j+1) * stirling(n, k, 2));
for(n=0, 17, print1(a(n),", "))      
