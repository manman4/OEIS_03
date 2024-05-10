T(n, k) = eulerphi(k*n)/eulerphi(k);
for(n=1, 14, for(k=1, n, print1(T(k,n-k+1),", ")))

