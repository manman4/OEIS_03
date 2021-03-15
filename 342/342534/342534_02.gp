M=60;

a(n) = sumdiv(n, d, eulerphi(n/d)*eulerphi(d)^2);
for(n=1, M, print1(a(n), ", "));