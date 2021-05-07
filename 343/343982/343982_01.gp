isok(n) = sumdiv(n, d,  Mod(d, n)^(n/d))==0;
M=1000000;
for(n=1, M, if(isok(n), print1(n, ", ")))