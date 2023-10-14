isok(n) = sumdiv(n, d, Mod(d, n)^(n/d))==0;
cnt=0;
for(n=1, 52734375, if(isok(n), cnt++; print(cnt, " ", n)))
