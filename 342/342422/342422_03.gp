M=50;

a(n) = sumdiv(n, d, eulerphi(d)*d^(n/d));
for(n=1, M, print1(a(n), ", "));