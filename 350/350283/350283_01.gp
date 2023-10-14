default(parisize, 200000000)

M=50;

a(n) = polcoef(prod(k=1, n, (x^k+1/x^k)^k), 0);
for(n=0, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b350283_1.txt", n, " ", i))
