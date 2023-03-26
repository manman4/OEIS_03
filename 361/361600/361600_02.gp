M=139;

T(n, k) = sum(j=0, n, (n+(k-1)*j)!/(k*j)! * binomial(n,j));
cnt=0;
for(n=0, M, for(k=0, n, write("/Users/xxx/Desktop/b361600_1.txt", cnt, " ", T(k, n-k)); cnt++));