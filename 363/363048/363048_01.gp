T(n, k) = sum(j=0, n, #partitions(n-k*j, k*j)); 

M=12;
for(n=0, M, for(k=0, n, print1(T(n,k),", ")))