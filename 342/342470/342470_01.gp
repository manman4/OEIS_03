M=50;

a(n) = sumdiv(n, d, eulerphi(d)^4);
for(n=1, M, print1(a(n), ", "));