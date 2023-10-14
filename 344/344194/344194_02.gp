M=26;

a(n) = sumdiv(n, d, eulerphi(n/d)*numdiv(d)^d);
for(n=1, M, print1(a(n), ", "));