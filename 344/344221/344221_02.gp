M=66;

a(n) = sumdiv(n, d, eulerphi(n/d)*numdiv(d^3));
for(n=1, M, print1(a(n), ", "));