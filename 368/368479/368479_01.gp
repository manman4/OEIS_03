T(n, k) = sum(j=0, n, 2^j*j^k);
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")));  
