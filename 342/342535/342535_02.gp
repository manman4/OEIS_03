M=60;

a(n) = sumdiv(n, d, eulerphi(n/d)*eulerphi(d)^3);
for(n=1, M, print1(a(n), ", "));