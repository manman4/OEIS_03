M=139;

T(n, k) = fromdigits(Vec(Pol(digits(n))^k)%10);
cnt=0;
for(n=0, M, for(k=0, n, write("/Users/xxx/Desktop/b361390_01.txt", cnt, " ", T(k, n-k)); cnt++))
