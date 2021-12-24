default(parisize, 200000000)

M=50;

a(n) = polcoef(prod(k=1, n, 1+x^(2*k))^n, n^2*(n+1)/2);
for(n=0, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b350282.txt", n, " ", i))
