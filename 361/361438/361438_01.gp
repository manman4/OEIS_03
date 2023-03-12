M=64;

cnt=1;
for(n=1, M, v= divisors(2^n-1); for(k=1, #v, write("/Users/xxx/Desktop/b361438.txt", cnt, " ", v[k]); cnt++))
