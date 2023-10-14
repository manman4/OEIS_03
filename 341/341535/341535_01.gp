default(realprecision, 1200);

a(n) = round(sqrt(prod(j=1, n, prod(k=1, n, 4*sin((2*j-1)*Pi/(2*n))^2+4*sin((2*k-1)*Pi/n)^2))));

M=100;
for(n=0, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b341535.txt", n, " ", i))