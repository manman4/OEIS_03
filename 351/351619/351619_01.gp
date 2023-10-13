M=101;

a(n) = my(f=factor(n)); sum(k=1, #f~, (-1)^f[k, 1]);
for(n=1, M, print1(a(n), ", "));