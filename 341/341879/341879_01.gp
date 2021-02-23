a(n) = my(m=0); for(k=1, n, if(sigma(k)==n, m=max(m, numdiv(k)))); m;

M=10000;
for(n=1, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b341879_1.txt", n, " ", i))