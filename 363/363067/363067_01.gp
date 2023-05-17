a(n) = sum(k=0, n\5, #partitions(n-5*k, 4*k));

M=55;
for(n=0, M, print1(a(n),", "))