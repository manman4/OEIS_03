M=61;

a(n) = sumdiv(n, d, eulerphi(n/d)*numdiv(d)^4);
for(n=1, M, print1(a(n), ", "));