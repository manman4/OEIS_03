T(n, k) = sum(j=0, n, #partitions(n-k*j, k*j)); 

M=52;
for(n=0, M, print1(T(n,5),", "))