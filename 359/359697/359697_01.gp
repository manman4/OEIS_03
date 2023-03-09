M=99;

T(n, k) = fromdigits(Vec(Pol(digits(n))*Pol(digits(k)))%10);
cnt=1;
for(n=1, M, for(k=1, n, write("/Users/xxx/Desktop/b359697_01.txt", cnt, " ", T(n, k)); cnt++))