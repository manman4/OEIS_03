isok(n) = my(f=factor(n)); sumdiv(n, d, Mod(d, n)^d)==1 & n>1 & !(#f~==1 & f[1, 1]>=f[1, 2]);
for(n=1, 1000000000000, if(isok(n), print1(n, ", ")));