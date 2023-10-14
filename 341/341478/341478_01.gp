default(realprecision, 120);

a(n) = round(sqrt(prod(j=1, n-1, prod(k=1, n, 4*sin(j*Pi/n)^2+4*sin((2*k-1)*Pi/(2*n))^2))));
for(n=0, 20, print1(a(n), ", "))