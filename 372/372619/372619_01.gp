T(n, k) = sum(j=1, n, eulerphi(k*j))/eulerphi(k);
for(n=1, 12, for(k=1, n, print1(T(k,n-k+1),", "))) 