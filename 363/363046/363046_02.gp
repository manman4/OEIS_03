T(n, k) = sum(j=0, n, #partitions(n-k*j, k*j)); 

M=51;
for(n=0, M, print1(T(n,4),", "))