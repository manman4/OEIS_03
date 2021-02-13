default(realprecision, 120);

a(n) = round(prod(j=1, n, prod(k=1, n+1, 4*sin((2*j-1)*Pi/n)^2+4*sin((2*k-1)*Pi/(n+1))^2))^(1/4));
for(n=0, 20, print1(a(n), ", "))