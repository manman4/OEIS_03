M=11;

T(n, k) = sum(j=0, n\2, k^j*abs(stirling(n, 2*j, 1)));
for(n=0, M, for(k=0, n, print1(T(k,n-k),", ")))   