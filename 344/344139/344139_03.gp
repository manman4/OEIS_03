M=53;

a(n) = sumdiv(n, d, eulerphi(n/d)*numdiv(d)^5);
for(n=1, M, print1(a(n), ", "));