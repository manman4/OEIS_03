a(n) = sum(k=0, n, 4^(n-k) * prod(j=0, k-1, 4*j+3) * abs(stirling(n, k, 1)));
for(n=0, 18, print1(a(n),", "))   
