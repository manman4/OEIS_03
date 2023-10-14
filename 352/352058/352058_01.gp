M=27;

a(n) = my(f=factor(n)); sum(k=1, #f~, n!/(f[k, 1]!*(n/f[k, 1])!));
for(n=1, M, print1(a(n), ", "));