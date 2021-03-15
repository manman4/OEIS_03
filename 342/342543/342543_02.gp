M=30;

a(n) = sumdiv(n, d, eulerphi(n/d)*eulerphi(d)^d);
for(n=1, M, print1(a(n), ", "));