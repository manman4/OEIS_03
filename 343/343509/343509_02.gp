M=26;

a(n) = sumdiv(n, d, eulerphi(n/d)*d^7);
for(n=1, M, print1(a(n),", ")); 