M=19;

a(n) = sumdiv(n, d, moebius(n/d)*d^n)/eulerphi(n);
for(n=1, M, print1(a(n), ", "));