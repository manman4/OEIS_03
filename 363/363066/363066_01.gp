a(n) = sum(k=0, n\4, #partitions(n-4*k, 3*k));

M=55;
for(n=0, M, print1(a(n),", "))