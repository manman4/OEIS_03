M=45;

a(n) = sumdiv(n, d, eulerphi(n/d)*d^3);
for(n=1, M, print1(a(n),", "));  