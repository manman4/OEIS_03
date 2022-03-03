M=23;

a(n) = my(f=factor(n)); sum(k=1, #f~, (n-1)!/(n/f[k, 1]-1)!);
for(n=1, M, print1(a(n), ", "));