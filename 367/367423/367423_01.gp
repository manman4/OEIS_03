a(n) = sum(k=0, n, 2^(n-k) * prod(j=0, k-1, 2*j+1) * abs(stirling(n, k, 1)));
for(n=0, 18, print1(a(n),", "))   
