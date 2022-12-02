M=11;

default(realprecision, 120);
T(n, k) = round(n*k*prod(a=1, n-1, prod(b=1, k-1, 4*sin(a*Pi/n)^2+4*sin(b*Pi/k)^2)));
a(n) = T(6, n);
for(n=1, M, print1(a(n), ", "));