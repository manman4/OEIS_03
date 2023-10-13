M=40000;

isok(n) = sum(k=1, n, Mod(-k, n)^k)==1;
for(n=1, M, if(isok(n), print1(n, ", ")));