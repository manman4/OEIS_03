default(realprecision, 2000);

b(n) = round(prod(j=1, n-1, prod(k=1, n, 4*sin(j*Pi/n)^2+4*sin((2*k-1)*Pi/(2*n))^2)));
c(n) = round(prod(j=1, n, prod(k=1, n, 4*sin((2*j-1)*Pi/(2*n))^2+4*sin((2*k-1)*Pi/(2*n))^2)));
a(n) = if(n==0, 1, 4*b(n)+c(n)/2);

M=50;
for(n=0, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b335586.txt", n, " ", i))