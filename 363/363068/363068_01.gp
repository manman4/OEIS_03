a(n) = sum(k=0, n\6, #partitions(n-6*k, 5*k));

M=56;
for(n=0, M, print1(a(n),", "))