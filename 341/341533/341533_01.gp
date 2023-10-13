default(realprecision, 120);

T(n, k) = round(sqrt(prod(a=1, n, prod(b=1, k, 4*sin((2*a-1)*Pi/(2*n))^2+4*sin((2*b-1)*Pi/k)^2))));
for(n=1, 10, for(k=1, n-1, print1(T(k, n-k), ", ")))
