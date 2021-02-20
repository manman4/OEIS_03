default(realprecision, 120);

T(n, k) = round(prod(a=1, n-1, prod(b=1, k, 4*sin(a*Pi/n)^2+4*sin((2*b-1)*Pi/(2*k))^2)));
for(n=1, 10, for(k=1, n-1, print1(T(k, n-k), ", ")))