isok(n) = sumdiv(n, d, Mod(d, n)^d)==1;
M=10000;
cnt=0;
n=0;
while(cnt<M, n++; if(isok(n), cnt++; write("/Users/xxx/Desktop/b343983_1.txt", cnt, " ", n)))