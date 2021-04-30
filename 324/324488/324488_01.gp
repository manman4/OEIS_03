M=30;

a001350(n) = fibonacci(n+1)+fibonacci(n-1)-1-(-1)^n;
a(n) = sumdiv(n, d, moebius(n/d)*a001350(d)^3); 
for(n=1, M, print1(a(n), ", "));