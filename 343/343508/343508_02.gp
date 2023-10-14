M=29;

a(n) = sumdiv(n, d, eulerphi(n/d)*d^6);
for(n=1, M, print1(a(n),", ")); 