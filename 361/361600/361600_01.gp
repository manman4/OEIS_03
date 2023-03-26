M=139;

T(n, k) = n!*sum(j=0, n, binomial(n+(k-1)*j, k*j)/j!);
cnt=0;
for(n=0, M, for(k=0, n, write("/Users/xxx/Desktop/b361600.txt", cnt, " ", T(k, n-k)); cnt++));