default(realprecision, 1100);

a(n) = round(sqrt(prod(j=1, n, prod(k=1, n-1, 4*sin((2*j-1)*Pi/(2*n))^2+4*sin(2*k*Pi/n)^2))));

M=100;
for(n=0, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b341782_1.txt", n, " ", i))