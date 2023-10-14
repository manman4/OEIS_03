a160893(n) = sumdiv(n, d, moebius(n/d)*d^5)/eulerphi(n);
a(n) = sumdiv(n, d, a160893(d));

M=10000;
for(n=1, M, write("/Users/xxx/Desktop/b344219_1.txt", n, " ", a(n)))