b(n, k) = sumdiv(n, d, moebius(n/d)*d^k)/eulerphi(n);
a(n) = sumdiv(n, d, b(d, n));

M=1000;
for(n=1, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b344201_1.txt", n, " ", i))